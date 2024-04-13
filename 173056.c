static void bnx2x_get_common_hwinfo(struct bnx2x *bp)
{
	u32 val, val2, val3, val4, id, boot_mode;
	u16 pmc;

	/* Get the chip revision id and number. */
	/* chip num:16-31, rev:12-15, metal:4-11, bond_id:0-3 */
	val = REG_RD(bp, MISC_REG_CHIP_NUM);
	id = ((val & 0xffff) << 16);
	val = REG_RD(bp, MISC_REG_CHIP_REV);
	id |= ((val & 0xf) << 12);

	/* Metal is read from PCI regs, but we can't access >=0x400 from
	 * the configuration space (so we need to reg_rd)
	 */
	val = REG_RD(bp, PCICFG_OFFSET + PCI_ID_VAL3);
	id |= (((val >> 24) & 0xf) << 4);
	val = REG_RD(bp, MISC_REG_BOND_ID);
	id |= (val & 0xf);
	bp->common.chip_id = id;

	/* force 57811 according to MISC register */
	if (REG_RD(bp, MISC_REG_CHIP_TYPE) & MISC_REG_CHIP_TYPE_57811_MASK) {
		if (CHIP_IS_57810(bp))
			bp->common.chip_id = (CHIP_NUM_57811 << 16) |
				(bp->common.chip_id & 0x0000FFFF);
		else if (CHIP_IS_57810_MF(bp))
			bp->common.chip_id = (CHIP_NUM_57811_MF << 16) |
				(bp->common.chip_id & 0x0000FFFF);
		bp->common.chip_id |= 0x1;
	}

	/* Set doorbell size */
	bp->db_size = (1 << BNX2X_DB_SHIFT);

	if (!CHIP_IS_E1x(bp)) {
		val = REG_RD(bp, MISC_REG_PORT4MODE_EN_OVWR);
		if ((val & 1) == 0)
			val = REG_RD(bp, MISC_REG_PORT4MODE_EN);
		else
			val = (val >> 1) & 1;
		BNX2X_DEV_INFO("chip is in %s\n", val ? "4_PORT_MODE" :
						       "2_PORT_MODE");
		bp->common.chip_port_mode = val ? CHIP_4_PORT_MODE :
						 CHIP_2_PORT_MODE;

		if (CHIP_MODE_IS_4_PORT(bp))
			bp->pfid = (bp->pf_num >> 1);	/* 0..3 */
		else
			bp->pfid = (bp->pf_num & 0x6);	/* 0, 2, 4, 6 */
	} else {
		bp->common.chip_port_mode = CHIP_PORT_MODE_NONE; /* N/A */
		bp->pfid = bp->pf_num;			/* 0..7 */
	}

	BNX2X_DEV_INFO("pf_id: %x", bp->pfid);

	bp->link_params.chip_id = bp->common.chip_id;
	BNX2X_DEV_INFO("chip ID is 0x%x\n", id);

	val = (REG_RD(bp, 0x2874) & 0x55);
	if ((bp->common.chip_id & 0x1) ||
	    (CHIP_IS_E1(bp) && val) || (CHIP_IS_E1H(bp) && (val == 0x55))) {
		bp->flags |= ONE_PORT_FLAG;
		BNX2X_DEV_INFO("single port device\n");
	}

	val = REG_RD(bp, MCP_REG_MCPR_NVM_CFG4);
	bp->common.flash_size = (BNX2X_NVRAM_1MB_SIZE <<
				 (val & MCPR_NVM_CFG4_FLASH_SIZE));
	BNX2X_DEV_INFO("flash_size 0x%x (%d)\n",
		       bp->common.flash_size, bp->common.flash_size);

	bnx2x_init_shmem(bp);

	bp->common.shmem2_base = REG_RD(bp, (BP_PATH(bp) ?
					MISC_REG_GENERIC_CR_1 :
					MISC_REG_GENERIC_CR_0));

	bp->link_params.shmem_base = bp->common.shmem_base;
	bp->link_params.shmem2_base = bp->common.shmem2_base;
	if (SHMEM2_RD(bp, size) >
	    (u32)offsetof(struct shmem2_region, lfa_host_addr[BP_PORT(bp)]))
		bp->link_params.lfa_base =
		REG_RD(bp, bp->common.shmem2_base +
		       (u32)offsetof(struct shmem2_region,
				     lfa_host_addr[BP_PORT(bp)]));
	else
		bp->link_params.lfa_base = 0;
	BNX2X_DEV_INFO("shmem offset 0x%x  shmem2 offset 0x%x\n",
		       bp->common.shmem_base, bp->common.shmem2_base);

	if (!bp->common.shmem_base) {
		BNX2X_DEV_INFO("MCP not active\n");
		bp->flags |= NO_MCP_FLAG;
		return;
	}

	bp->common.hw_config = SHMEM_RD(bp, dev_info.shared_hw_config.config);
	BNX2X_DEV_INFO("hw_config 0x%08x\n", bp->common.hw_config);

	bp->link_params.hw_led_mode = ((bp->common.hw_config &
					SHARED_HW_CFG_LED_MODE_MASK) >>
				       SHARED_HW_CFG_LED_MODE_SHIFT);

	bp->link_params.feature_config_flags = 0;
	val = SHMEM_RD(bp, dev_info.shared_feature_config.config);
	if (val & SHARED_FEAT_CFG_OVERRIDE_PREEMPHASIS_CFG_ENABLED)
		bp->link_params.feature_config_flags |=
				FEATURE_CONFIG_OVERRIDE_PREEMPHASIS_ENABLED;
	else
		bp->link_params.feature_config_flags &=
				~FEATURE_CONFIG_OVERRIDE_PREEMPHASIS_ENABLED;

	val = SHMEM_RD(bp, dev_info.bc_rev) >> 8;
	bp->common.bc_ver = val;
	BNX2X_DEV_INFO("bc_ver %X\n", val);
	if (val < BNX2X_BC_VER) {
		/* for now only warn
		 * later we might need to enforce this */
		BNX2X_ERR("This driver needs bc_ver %X but found %X, please upgrade BC\n",
			  BNX2X_BC_VER, val);
	}
	bp->link_params.feature_config_flags |=
				(val >= REQ_BC_VER_4_VRFY_FIRST_PHY_OPT_MDL) ?
				FEATURE_CONFIG_BC_SUPPORTS_OPT_MDL_VRFY : 0;

	bp->link_params.feature_config_flags |=
		(val >= REQ_BC_VER_4_VRFY_SPECIFIC_PHY_OPT_MDL) ?
		FEATURE_CONFIG_BC_SUPPORTS_DUAL_PHY_OPT_MDL_VRFY : 0;
	bp->link_params.feature_config_flags |=
		(val >= REQ_BC_VER_4_VRFY_AFEX_SUPPORTED) ?
		FEATURE_CONFIG_BC_SUPPORTS_AFEX : 0;
	bp->link_params.feature_config_flags |=
		(val >= REQ_BC_VER_4_SFP_TX_DISABLE_SUPPORTED) ?
		FEATURE_CONFIG_BC_SUPPORTS_SFP_TX_DISABLED : 0;

	bp->link_params.feature_config_flags |=
		(val >= REQ_BC_VER_4_MT_SUPPORTED) ?
		FEATURE_CONFIG_MT_SUPPORT : 0;

	bp->flags |= (val >= REQ_BC_VER_4_PFC_STATS_SUPPORTED) ?
			BC_SUPPORTS_PFC_STATS : 0;

	bp->flags |= (val >= REQ_BC_VER_4_FCOE_FEATURES) ?
			BC_SUPPORTS_FCOE_FEATURES : 0;

	bp->flags |= (val >= REQ_BC_VER_4_DCBX_ADMIN_MSG_NON_PMF) ?
			BC_SUPPORTS_DCBX_MSG_NON_PMF : 0;

	bp->flags |= (val >= REQ_BC_VER_4_RMMOD_CMD) ?
			BC_SUPPORTS_RMMOD_CMD : 0;

	boot_mode = SHMEM_RD(bp,
			dev_info.port_feature_config[BP_PORT(bp)].mba_config) &
			PORT_FEATURE_MBA_BOOT_AGENT_TYPE_MASK;
	switch (boot_mode) {
	case PORT_FEATURE_MBA_BOOT_AGENT_TYPE_PXE:
		bp->common.boot_mode = FEATURE_ETH_BOOTMODE_PXE;
		break;
	case PORT_FEATURE_MBA_BOOT_AGENT_TYPE_ISCSIB:
		bp->common.boot_mode = FEATURE_ETH_BOOTMODE_ISCSI;
		break;
	case PORT_FEATURE_MBA_BOOT_AGENT_TYPE_FCOE_BOOT:
		bp->common.boot_mode = FEATURE_ETH_BOOTMODE_FCOE;
		break;
	case PORT_FEATURE_MBA_BOOT_AGENT_TYPE_NONE:
		bp->common.boot_mode = FEATURE_ETH_BOOTMODE_NONE;
		break;
	}

	pci_read_config_word(bp->pdev, bp->pdev->pm_cap + PCI_PM_PMC, &pmc);
	bp->flags |= (pmc & PCI_PM_CAP_PME_D3cold) ? 0 : NO_WOL_FLAG;

	BNX2X_DEV_INFO("%sWoL capable\n",
		       (bp->flags & NO_WOL_FLAG) ? "not " : "");

	val = SHMEM_RD(bp, dev_info.shared_hw_config.part_num);
	val2 = SHMEM_RD(bp, dev_info.shared_hw_config.part_num[4]);
	val3 = SHMEM_RD(bp, dev_info.shared_hw_config.part_num[8]);
	val4 = SHMEM_RD(bp, dev_info.shared_hw_config.part_num[12]);

	dev_info(&bp->pdev->dev, "part number %X-%X-%X-%X\n",
		 val, val2, val3, val4);
}