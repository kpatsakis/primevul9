static void bnx2x_drv_info_iscsi_stat(struct bnx2x *bp)
{
	struct bnx2x_dcbx_app_params *app = &bp->dcbx_port_params.app;
	struct iscsi_stats_info *iscsi_stat =
		&bp->slowpath->drv_info_to_mcp.iscsi_stat;

	if (!CNIC_LOADED(bp))
		return;

	memcpy(iscsi_stat->mac_local + MAC_PAD, bp->cnic_eth_dev.iscsi_mac,
	       ETH_ALEN);

	iscsi_stat->qos_priority =
		app->traffic_type_priority[LLFC_TRAFFIC_TYPE_ISCSI];

	/* ask L5 driver to add data to the struct */
	bnx2x_cnic_notify(bp, CNIC_CTL_ISCSI_STATS_GET_CMD);
}