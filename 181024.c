int ath6kl_wmi_set_roam_lrssi_cmd(struct wmi *wmi, u8 lrssi)
{
	struct sk_buff *skb;
	struct roam_ctrl_cmd *cmd;

	skb = ath6kl_wmi_get_new_buf(sizeof(*cmd));
	if (!skb)
		return -ENOMEM;

	cmd = (struct roam_ctrl_cmd *) skb->data;

	cmd->info.params.lrssi_scan_period = cpu_to_le16(DEF_LRSSI_SCAN_PERIOD);
	cmd->info.params.lrssi_scan_threshold = a_cpu_to_sle16(lrssi +
						       DEF_SCAN_FOR_ROAM_INTVL);
	cmd->info.params.lrssi_roam_threshold = a_cpu_to_sle16(lrssi);
	cmd->info.params.roam_rssi_floor = DEF_LRSSI_ROAM_FLOOR;
	cmd->roam_ctrl = WMI_SET_LRSSI_SCAN_PARAMS;

	return ath6kl_wmi_cmd_send(wmi, 0, skb, WMI_SET_ROAM_CTRL_CMDID,
			    NO_SYNC_WMIFLAG);
}