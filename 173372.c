static void bnx2x_fan_failure(struct bnx2x *bp)
{
	int port = BP_PORT(bp);
	u32 ext_phy_config;
	/* mark the failure */
	ext_phy_config =
		SHMEM_RD(bp,
			 dev_info.port_hw_config[port].external_phy_config);

	ext_phy_config &= ~PORT_HW_CFG_XGXS_EXT_PHY_TYPE_MASK;
	ext_phy_config |= PORT_HW_CFG_XGXS_EXT_PHY_TYPE_FAILURE;
	SHMEM_WR(bp, dev_info.port_hw_config[port].external_phy_config,
		 ext_phy_config);

	/* log the failure */
	netdev_err(bp->dev, "Fan Failure on Network Controller has caused the driver to shutdown the card to prevent permanent damage.\n"
			    "Please contact OEM Support for assistance\n");

	/* Schedule device reset (unload)
	 * This is due to some boards consuming sufficient power when driver is
	 * up to overheat if fan fails.
	 */
	bnx2x_schedule_sp_rtnl(bp, BNX2X_SP_RTNL_FAN_FAILURE, 0);
}