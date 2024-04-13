int mwifiex_cmd_802_11_bg_scan_query(struct host_cmd_ds_command *cmd)
{
	struct host_cmd_ds_802_11_bg_scan_query *bg_query =
		&cmd->params.bg_scan_query;

	cmd->command = cpu_to_le16(HostCmd_CMD_802_11_BG_SCAN_QUERY);
	cmd->size = cpu_to_le16(sizeof(struct host_cmd_ds_802_11_bg_scan_query)
				+ S_DS_GEN);

	bg_query->flush = 1;

	return 0;
}