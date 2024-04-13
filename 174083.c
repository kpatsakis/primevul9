int mwifiex_cmd_802_11_scan_ext(struct mwifiex_private *priv,
				struct host_cmd_ds_command *cmd,
				void *data_buf)
{
	struct host_cmd_ds_802_11_scan_ext *ext_scan = &cmd->params.ext_scan;
	struct mwifiex_scan_cmd_config *scan_cfg = data_buf;

	memcpy(ext_scan->tlv_buffer, scan_cfg->tlv_buf, scan_cfg->tlv_buf_len);

	cmd->command = cpu_to_le16(HostCmd_CMD_802_11_SCAN_EXT);

	/* Size is equal to the sizeof(fixed portions) + the TLV len + header */
	cmd->size = cpu_to_le16((u16)(sizeof(ext_scan->reserved)
				      + scan_cfg->tlv_buf_len + S_DS_GEN));

	return 0;
}