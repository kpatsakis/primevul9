static int bnx2x_drv_ctl(struct net_device *dev, struct drv_ctl_info *ctl)
{
	struct bnx2x *bp = netdev_priv(dev);
	int rc = 0;

	switch (ctl->cmd) {
	case DRV_CTL_CTXTBL_WR_CMD: {
		u32 index = ctl->data.io.offset;
		dma_addr_t addr = ctl->data.io.dma_addr;

		bnx2x_ilt_wr(bp, index, addr);
		break;
	}

	case DRV_CTL_RET_L5_SPQ_CREDIT_CMD: {
		int count = ctl->data.credit.credit_count;

		bnx2x_cnic_sp_post(bp, count);
		break;
	}

	/* rtnl_lock is held.  */
	case DRV_CTL_START_L2_CMD: {
		struct cnic_eth_dev *cp = &bp->cnic_eth_dev;
		unsigned long sp_bits = 0;

		/* Configure the iSCSI classification object */
		bnx2x_init_mac_obj(bp, &bp->iscsi_l2_mac_obj,
				   cp->iscsi_l2_client_id,
				   cp->iscsi_l2_cid, BP_FUNC(bp),
				   bnx2x_sp(bp, mac_rdata),
				   bnx2x_sp_mapping(bp, mac_rdata),
				   BNX2X_FILTER_MAC_PENDING,
				   &bp->sp_state, BNX2X_OBJ_TYPE_RX,
				   &bp->macs_pool);

		/* Set iSCSI MAC address */
		rc = bnx2x_set_iscsi_eth_mac_addr(bp);
		if (rc)
			break;

		mmiowb();
		barrier();

		/* Start accepting on iSCSI L2 ring */

		netif_addr_lock_bh(dev);
		bnx2x_set_iscsi_eth_rx_mode(bp, true);
		netif_addr_unlock_bh(dev);

		/* bits to wait on */
		__set_bit(BNX2X_FILTER_RX_MODE_PENDING, &sp_bits);
		__set_bit(BNX2X_FILTER_ISCSI_ETH_START_SCHED, &sp_bits);

		if (!bnx2x_wait_sp_comp(bp, sp_bits))
			BNX2X_ERR("rx_mode completion timed out!\n");

		break;
	}

	/* rtnl_lock is held.  */
	case DRV_CTL_STOP_L2_CMD: {
		unsigned long sp_bits = 0;

		/* Stop accepting on iSCSI L2 ring */
		netif_addr_lock_bh(dev);
		bnx2x_set_iscsi_eth_rx_mode(bp, false);
		netif_addr_unlock_bh(dev);

		/* bits to wait on */
		__set_bit(BNX2X_FILTER_RX_MODE_PENDING, &sp_bits);
		__set_bit(BNX2X_FILTER_ISCSI_ETH_STOP_SCHED, &sp_bits);

		if (!bnx2x_wait_sp_comp(bp, sp_bits))
			BNX2X_ERR("rx_mode completion timed out!\n");

		mmiowb();
		barrier();

		/* Unset iSCSI L2 MAC */
		rc = bnx2x_del_all_macs(bp, &bp->iscsi_l2_mac_obj,
					BNX2X_ISCSI_ETH_MAC, true);
		break;
	}
	case DRV_CTL_RET_L2_SPQ_CREDIT_CMD: {
		int count = ctl->data.credit.credit_count;

		smp_mb__before_atomic();
		atomic_add(count, &bp->cq_spq_left);
		smp_mb__after_atomic();
		break;
	}
	case DRV_CTL_ULP_REGISTER_CMD: {
		int ulp_type = ctl->data.register_data.ulp_type;

		if (CHIP_IS_E3(bp)) {
			int idx = BP_FW_MB_IDX(bp);
			u32 cap = SHMEM2_RD(bp, drv_capabilities_flag[idx]);
			int path = BP_PATH(bp);
			int port = BP_PORT(bp);
			int i;
			u32 scratch_offset;
			u32 *host_addr;

			/* first write capability to shmem2 */
			if (ulp_type == CNIC_ULP_ISCSI)
				cap |= DRV_FLAGS_CAPABILITIES_LOADED_ISCSI;
			else if (ulp_type == CNIC_ULP_FCOE)
				cap |= DRV_FLAGS_CAPABILITIES_LOADED_FCOE;
			SHMEM2_WR(bp, drv_capabilities_flag[idx], cap);

			if ((ulp_type != CNIC_ULP_FCOE) ||
			    (!SHMEM2_HAS(bp, ncsi_oem_data_addr)) ||
			    (!(bp->flags &  BC_SUPPORTS_FCOE_FEATURES)))
				break;

			/* if reached here - should write fcoe capabilities */
			scratch_offset = SHMEM2_RD(bp, ncsi_oem_data_addr);
			if (!scratch_offset)
				break;
			scratch_offset += offsetof(struct glob_ncsi_oem_data,
						   fcoe_features[path][port]);
			host_addr = (u32 *) &(ctl->data.register_data.
					      fcoe_features);
			for (i = 0; i < sizeof(struct fcoe_capabilities);
			     i += 4)
				REG_WR(bp, scratch_offset + i,
				       *(host_addr + i/4));
		}
		bnx2x_schedule_sp_rtnl(bp, BNX2X_SP_RTNL_GET_DRV_VERSION, 0);
		break;
	}

	case DRV_CTL_ULP_UNREGISTER_CMD: {
		int ulp_type = ctl->data.ulp_type;

		if (CHIP_IS_E3(bp)) {
			int idx = BP_FW_MB_IDX(bp);
			u32 cap;

			cap = SHMEM2_RD(bp, drv_capabilities_flag[idx]);
			if (ulp_type == CNIC_ULP_ISCSI)
				cap &= ~DRV_FLAGS_CAPABILITIES_LOADED_ISCSI;
			else if (ulp_type == CNIC_ULP_FCOE)
				cap &= ~DRV_FLAGS_CAPABILITIES_LOADED_FCOE;
			SHMEM2_WR(bp, drv_capabilities_flag[idx], cap);
		}
		bnx2x_schedule_sp_rtnl(bp, BNX2X_SP_RTNL_GET_DRV_VERSION, 0);
		break;
	}

	default:
		BNX2X_ERR("unknown command %x\n", ctl->cmd);
		rc = -EINVAL;
	}

	/* For storage-only interfaces, change driver state */
	if (IS_MF_SD_STORAGE_PERSONALITY_ONLY(bp)) {
		switch (ctl->drv_state) {
		case DRV_NOP:
			break;
		case DRV_ACTIVE:
			bnx2x_set_os_driver_state(bp,
						  OS_DRIVER_STATE_ACTIVE);
			break;
		case DRV_INACTIVE:
			bnx2x_set_os_driver_state(bp,
						  OS_DRIVER_STATE_DISABLED);
			break;
		case DRV_UNLOADED:
			bnx2x_set_os_driver_state(bp,
						  OS_DRIVER_STATE_NOT_LOADED);
			break;
		default:
		BNX2X_ERR("Unknown cnic driver state: %d\n", ctl->drv_state);
		}
	}

	return rc;
}