static void bnx2x_get_port_hwinfo(struct bnx2x *bp)
{
	int port = BP_PORT(bp);
	u32 config;
	u32 ext_phy_type, ext_phy_config, eee_mode;

	bp->link_params.bp = bp;
	bp->link_params.port = port;

	bp->link_params.lane_config =
		SHMEM_RD(bp, dev_info.port_hw_config[port].lane_config);

	bp->link_params.speed_cap_mask[0] =
		SHMEM_RD(bp,
			 dev_info.port_hw_config[port].speed_capability_mask) &
		PORT_HW_CFG_SPEED_CAPABILITY_D0_MASK;
	bp->link_params.speed_cap_mask[1] =
		SHMEM_RD(bp,
			 dev_info.port_hw_config[port].speed_capability_mask2) &
		PORT_HW_CFG_SPEED_CAPABILITY_D0_MASK;
	bp->port.link_config[0] =
		SHMEM_RD(bp, dev_info.port_feature_config[port].link_config);

	bp->port.link_config[1] =
		SHMEM_RD(bp, dev_info.port_feature_config[port].link_config2);

	bp->link_params.multi_phy_config =
		SHMEM_RD(bp, dev_info.port_hw_config[port].multi_phy_config);
	/* If the device is capable of WoL, set the default state according
	 * to the HW
	 */
	config = SHMEM_RD(bp, dev_info.port_feature_config[port].config);
	bp->wol = (!(bp->flags & NO_WOL_FLAG) &&
		   (config & PORT_FEATURE_WOL_ENABLED));

	if ((config & PORT_FEAT_CFG_STORAGE_PERSONALITY_MASK) ==
	    PORT_FEAT_CFG_STORAGE_PERSONALITY_FCOE && !IS_MF(bp))
		bp->flags |= NO_ISCSI_FLAG;
	if ((config & PORT_FEAT_CFG_STORAGE_PERSONALITY_MASK) ==
	    PORT_FEAT_CFG_STORAGE_PERSONALITY_ISCSI && !(IS_MF(bp)))
		bp->flags |= NO_FCOE_FLAG;

	BNX2X_DEV_INFO("lane_config 0x%08x  speed_cap_mask0 0x%08x  link_config0 0x%08x\n",
		       bp->link_params.lane_config,
		       bp->link_params.speed_cap_mask[0],
		       bp->port.link_config[0]);

	bp->link_params.switch_cfg = (bp->port.link_config[0] &
				      PORT_FEATURE_CONNECTED_SWITCH_MASK);
	bnx2x_phy_probe(&bp->link_params);
	bnx2x_link_settings_supported(bp, bp->link_params.switch_cfg);

	bnx2x_link_settings_requested(bp);

	/*
	 * If connected directly, work with the internal PHY, otherwise, work
	 * with the external PHY
	 */
	ext_phy_config =
		SHMEM_RD(bp,
			 dev_info.port_hw_config[port].external_phy_config);
	ext_phy_type = XGXS_EXT_PHY_TYPE(ext_phy_config);
	if (ext_phy_type == PORT_HW_CFG_XGXS_EXT_PHY_TYPE_DIRECT)
		bp->mdio.prtad = bp->port.phy_addr;

	else if ((ext_phy_type != PORT_HW_CFG_XGXS_EXT_PHY_TYPE_FAILURE) &&
		 (ext_phy_type != PORT_HW_CFG_XGXS_EXT_PHY_TYPE_NOT_CONN))
		bp->mdio.prtad =
			XGXS_EXT_PHY_ADDR(ext_phy_config);

	/* Configure link feature according to nvram value */
	eee_mode = (((SHMEM_RD(bp, dev_info.
		      port_feature_config[port].eee_power_mode)) &
		     PORT_FEAT_CFG_EEE_POWER_MODE_MASK) >>
		    PORT_FEAT_CFG_EEE_POWER_MODE_SHIFT);
	if (eee_mode != PORT_FEAT_CFG_EEE_POWER_MODE_DISABLED) {
		bp->link_params.eee_mode = EEE_MODE_ADV_LPI |
					   EEE_MODE_ENABLE_LPI |
					   EEE_MODE_OUTPUT_TIME;
	} else {
		bp->link_params.eee_mode = 0;
	}
}