int ath6kl_wmi_force_roam_cmd(struct wmi *wmi, const u8 *bssid)
{
	struct sk_buff *skb;
	struct roam_ctrl_cmd *cmd;

	skb = ath6kl_wmi_get_new_buf(sizeof(*cmd));
	if (!skb)
		return -ENOMEM;

	cmd = (struct roam_ctrl_cmd *) skb->data;

	memcpy(cmd->info.bssid, bssid, ETH_ALEN);
	cmd->roam_ctrl = WMI_FORCE_ROAM;

	ath6kl_dbg(ATH6KL_DBG_WMI, "force roam to %pM\n", bssid);
	return ath6kl_wmi_cmd_send(wmi, 0, skb, WMI_SET_ROAM_CTRL_CMDID,
				   NO_SYNC_WMIFLAG);
}