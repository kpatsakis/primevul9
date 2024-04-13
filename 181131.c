int ath6kl_wmi_set_roam_mode_cmd(struct wmi *wmi, enum wmi_roam_mode mode)
{
	struct sk_buff *skb;
	struct roam_ctrl_cmd *cmd;

	skb = ath6kl_wmi_get_new_buf(sizeof(*cmd));
	if (!skb)
		return -ENOMEM;

	cmd = (struct roam_ctrl_cmd *) skb->data;

	cmd->info.roam_mode = mode;
	cmd->roam_ctrl = WMI_SET_ROAM_MODE;

	ath6kl_dbg(ATH6KL_DBG_WMI, "set roam mode %d\n", mode);
	return ath6kl_wmi_cmd_send(wmi, 0, skb, WMI_SET_ROAM_CTRL_CMDID,
				   NO_SYNC_WMIFLAG);
}