int mwifiex_cmd_802_11_scan(struct host_cmd_ds_command *cmd,
			    struct mwifiex_scan_cmd_config *scan_cfg)
{
	struct host_cmd_ds_802_11_scan *scan_cmd = &cmd->params.scan;

	/* Set fixed field variables in scan command */
	scan_cmd->bss_mode = scan_cfg->bss_mode;
	memcpy(scan_cmd->bssid, scan_cfg->specific_bssid,
	       sizeof(scan_cmd->bssid));
	memcpy(scan_cmd->tlv_buffer, scan_cfg->tlv_buf, scan_cfg->tlv_buf_len);

	cmd->command = cpu_to_le16(HostCmd_CMD_802_11_SCAN);

	/* Size is equal to the sizeof(fixed portions) + the TLV len + header */
	cmd->size = cpu_to_le16((u16) (sizeof(scan_cmd->bss_mode)
					  + sizeof(scan_cmd->bssid)
					  + scan_cfg->tlv_buf_len + S_DS_GEN));

	return 0;
}