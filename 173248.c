static int bnx2x_get_hwinfo(struct bnx2x *bp)
{
	int /*abs*/func = BP_ABS_FUNC(bp);
	int vn, mfw_vn;
	u32 val = 0, val2 = 0;
	int rc = 0;

	/* Validate that chip access is feasible */
	if (REG_RD(bp, MISC_REG_CHIP_NUM) == 0xffffffff) {
		dev_err(&bp->pdev->dev,
			"Chip read returns all Fs. Preventing probe from continuing\n");
		return -EINVAL;
	}

	bnx2x_get_common_hwinfo(bp);

	/*
	 * initialize IGU parameters
	 */
	if (CHIP_IS_E1x(bp)) {
		bp->common.int_block = INT_BLOCK_HC;

		bp->igu_dsb_id = DEF_SB_IGU_ID;
		bp->igu_base_sb = 0;
	} else {
		bp->common.int_block = INT_BLOCK_IGU;

		/* do not allow device reset during IGU info processing */
		bnx2x_acquire_hw_lock(bp, HW_LOCK_RESOURCE_RESET);

		val = REG_RD(bp, IGU_REG_BLOCK_CONFIGURATION);

		if (val & IGU_BLOCK_CONFIGURATION_REG_BACKWARD_COMP_EN) {
			int tout = 5000;

			BNX2X_DEV_INFO("FORCING Normal Mode\n");

			val &= ~(IGU_BLOCK_CONFIGURATION_REG_BACKWARD_COMP_EN);
			REG_WR(bp, IGU_REG_BLOCK_CONFIGURATION, val);
			REG_WR(bp, IGU_REG_RESET_MEMORIES, 0x7f);

			while (tout && REG_RD(bp, IGU_REG_RESET_MEMORIES)) {
				tout--;
				usleep_range(1000, 2000);
			}

			if (REG_RD(bp, IGU_REG_RESET_MEMORIES)) {
				dev_err(&bp->pdev->dev,
					"FORCING Normal Mode failed!!!\n");
				bnx2x_release_hw_lock(bp,
						      HW_LOCK_RESOURCE_RESET);
				return -EPERM;
			}
		}

		if (val & IGU_BLOCK_CONFIGURATION_REG_BACKWARD_COMP_EN) {
			BNX2X_DEV_INFO("IGU Backward Compatible Mode\n");
			bp->common.int_block |= INT_BLOCK_MODE_BW_COMP;
		} else
			BNX2X_DEV_INFO("IGU Normal Mode\n");

		rc = bnx2x_get_igu_cam_info(bp);
		bnx2x_release_hw_lock(bp, HW_LOCK_RESOURCE_RESET);
		if (rc)
			return rc;
	}

	/*
	 * set base FW non-default (fast path) status block id, this value is
	 * used to initialize the fw_sb_id saved on the fp/queue structure to
	 * determine the id used by the FW.
	 */
	if (CHIP_IS_E1x(bp))
		bp->base_fw_ndsb = BP_PORT(bp) * FP_SB_MAX_E1x + BP_L_ID(bp);
	else /*
	      * 57712 - we currently use one FW SB per IGU SB (Rx and Tx of
	      * the same queue are indicated on the same IGU SB). So we prefer
	      * FW and IGU SBs to be the same value.
	      */
		bp->base_fw_ndsb = bp->igu_base_sb;

	BNX2X_DEV_INFO("igu_dsb_id %d  igu_base_sb %d  igu_sb_cnt %d\n"
		       "base_fw_ndsb %d\n", bp->igu_dsb_id, bp->igu_base_sb,
		       bp->igu_sb_cnt, bp->base_fw_ndsb);

	/*
	 * Initialize MF configuration
	 */

	bp->mf_ov = 0;
	bp->mf_mode = 0;
	bp->mf_sub_mode = 0;
	vn = BP_VN(bp);
	mfw_vn = BP_FW_MB_IDX(bp);

	if (!CHIP_IS_E1(bp) && !BP_NOMCP(bp)) {
		BNX2X_DEV_INFO("shmem2base 0x%x, size %d, mfcfg offset %d\n",
			       bp->common.shmem2_base, SHMEM2_RD(bp, size),
			      (u32)offsetof(struct shmem2_region, mf_cfg_addr));

		if (SHMEM2_HAS(bp, mf_cfg_addr))
			bp->common.mf_cfg_base = SHMEM2_RD(bp, mf_cfg_addr);
		else
			bp->common.mf_cfg_base = bp->common.shmem_base +
				offsetof(struct shmem_region, func_mb) +
				E1H_FUNC_MAX * sizeof(struct drv_func_mb);
		/*
		 * get mf configuration:
		 * 1. Existence of MF configuration
		 * 2. MAC address must be legal (check only upper bytes)
		 *    for  Switch-Independent mode;
		 *    OVLAN must be legal for Switch-Dependent mode
		 * 3. SF_MODE configures specific MF mode
		 */
		if (bp->common.mf_cfg_base != SHMEM_MF_CFG_ADDR_NONE) {
			/* get mf configuration */
			val = SHMEM_RD(bp,
				       dev_info.shared_feature_config.config);
			val &= SHARED_FEAT_CFG_FORCE_SF_MODE_MASK;

			switch (val) {
			case SHARED_FEAT_CFG_FORCE_SF_MODE_SWITCH_INDEPT:
				validate_set_si_mode(bp);
				break;
			case SHARED_FEAT_CFG_FORCE_SF_MODE_AFEX_MODE:
				if ((!CHIP_IS_E1x(bp)) &&
				    (MF_CFG_RD(bp, func_mf_config[func].
					       mac_upper) != 0xffff) &&
				    (SHMEM2_HAS(bp,
						afex_driver_support))) {
					bp->mf_mode = MULTI_FUNCTION_AFEX;
					bp->mf_config[vn] = MF_CFG_RD(bp,
						func_mf_config[func].config);
				} else {
					BNX2X_DEV_INFO("can not configure afex mode\n");
				}
				break;
			case SHARED_FEAT_CFG_FORCE_SF_MODE_MF_ALLOWED:
				/* get OV configuration */
				val = MF_CFG_RD(bp,
					func_mf_config[FUNC_0].e1hov_tag);
				val &= FUNC_MF_CFG_E1HOV_TAG_MASK;

				if (val != FUNC_MF_CFG_E1HOV_TAG_DEFAULT) {
					bp->mf_mode = MULTI_FUNCTION_SD;
					bp->mf_config[vn] = MF_CFG_RD(bp,
						func_mf_config[func].config);
				} else
					BNX2X_DEV_INFO("illegal OV for SD\n");
				break;
			case SHARED_FEAT_CFG_FORCE_SF_MODE_BD_MODE:
				bp->mf_mode = MULTI_FUNCTION_SD;
				bp->mf_sub_mode = SUB_MF_MODE_BD;
				bp->mf_config[vn] =
					MF_CFG_RD(bp,
						  func_mf_config[func].config);

				if (SHMEM2_HAS(bp, mtu_size)) {
					int mtu_idx = BP_FW_MB_IDX(bp);
					u16 mtu_size;
					u32 mtu;

					mtu = SHMEM2_RD(bp, mtu_size[mtu_idx]);
					mtu_size = (u16)mtu;
					DP(NETIF_MSG_IFUP, "Read MTU size %04x [%08x]\n",
					   mtu_size, mtu);

					/* if valid: update device mtu */
					if ((mtu_size >= ETH_MIN_PACKET_SIZE) &&
					    (mtu_size <=
					     ETH_MAX_JUMBO_PACKET_SIZE))
						bp->dev->mtu = mtu_size;
				}
				break;
			case SHARED_FEAT_CFG_FORCE_SF_MODE_UFP_MODE:
				bp->mf_mode = MULTI_FUNCTION_SD;
				bp->mf_sub_mode = SUB_MF_MODE_UFP;
				bp->mf_config[vn] =
					MF_CFG_RD(bp,
						  func_mf_config[func].config);
				break;
			case SHARED_FEAT_CFG_FORCE_SF_MODE_FORCED_SF:
				bp->mf_config[vn] = 0;
				break;
			case SHARED_FEAT_CFG_FORCE_SF_MODE_EXTENDED_MODE:
				val2 = SHMEM_RD(bp,
					dev_info.shared_hw_config.config_3);
				val2 &= SHARED_HW_CFG_EXTENDED_MF_MODE_MASK;
				switch (val2) {
				case SHARED_HW_CFG_EXTENDED_MF_MODE_NPAR1_DOT_5:
					validate_set_si_mode(bp);
					bp->mf_sub_mode =
							SUB_MF_MODE_NPAR1_DOT_5;
					break;
				default:
					/* Unknown configuration */
					bp->mf_config[vn] = 0;
					BNX2X_DEV_INFO("unknown extended MF mode 0x%x\n",
						       val);
				}
				break;
			default:
				/* Unknown configuration: reset mf_config */
				bp->mf_config[vn] = 0;
				BNX2X_DEV_INFO("unknown MF mode 0x%x\n", val);
			}
		}

		BNX2X_DEV_INFO("%s function mode\n",
			       IS_MF(bp) ? "multi" : "single");

		switch (bp->mf_mode) {
		case MULTI_FUNCTION_SD:
			val = MF_CFG_RD(bp, func_mf_config[func].e1hov_tag) &
			      FUNC_MF_CFG_E1HOV_TAG_MASK;
			if (val != FUNC_MF_CFG_E1HOV_TAG_DEFAULT) {
				bp->mf_ov = val;
				bp->path_has_ovlan = true;

				BNX2X_DEV_INFO("MF OV for func %d is %d (0x%04x)\n",
					       func, bp->mf_ov, bp->mf_ov);
			} else if ((bp->mf_sub_mode == SUB_MF_MODE_UFP) ||
				   (bp->mf_sub_mode == SUB_MF_MODE_BD)) {
				dev_err(&bp->pdev->dev,
					"Unexpected - no valid MF OV for func %d in UFP/BD mode\n",
					func);
				bp->path_has_ovlan = true;
			} else {
				dev_err(&bp->pdev->dev,
					"No valid MF OV for func %d, aborting\n",
					func);
				return -EPERM;
			}
			break;
		case MULTI_FUNCTION_AFEX:
			BNX2X_DEV_INFO("func %d is in MF afex mode\n", func);
			break;
		case MULTI_FUNCTION_SI:
			BNX2X_DEV_INFO("func %d is in MF switch-independent mode\n",
				       func);
			break;
		default:
			if (vn) {
				dev_err(&bp->pdev->dev,
					"VN %d is in a single function mode, aborting\n",
					vn);
				return -EPERM;
			}
			break;
		}

		/* check if other port on the path needs ovlan:
		 * Since MF configuration is shared between ports
		 * Possible mixed modes are only
		 * {SF, SI} {SF, SD} {SD, SF} {SI, SF}
		 */
		if (CHIP_MODE_IS_4_PORT(bp) &&
		    !bp->path_has_ovlan &&
		    !IS_MF(bp) &&
		    bp->common.mf_cfg_base != SHMEM_MF_CFG_ADDR_NONE) {
			u8 other_port = !BP_PORT(bp);
			u8 other_func = BP_PATH(bp) + 2*other_port;
			val = MF_CFG_RD(bp,
					func_mf_config[other_func].e1hov_tag);
			if (val != FUNC_MF_CFG_E1HOV_TAG_DEFAULT)
				bp->path_has_ovlan = true;
		}
	}

	/* adjust igu_sb_cnt to MF for E1H */
	if (CHIP_IS_E1H(bp) && IS_MF(bp))
		bp->igu_sb_cnt = min_t(u8, bp->igu_sb_cnt, E1H_MAX_MF_SB_COUNT);

	/* port info */
	bnx2x_get_port_hwinfo(bp);

	/* Get MAC addresses */
	bnx2x_get_mac_hwinfo(bp);

	bnx2x_get_cnic_info(bp);

	return rc;
}