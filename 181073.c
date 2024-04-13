int ath6kl_wmi_ap_hidden_ssid(struct wmi *wmi, u8 if_idx, bool enable)
{
	struct sk_buff *skb;
	struct wmi_ap_hidden_ssid_cmd *cmd;

	skb = ath6kl_wmi_get_new_buf(sizeof(*cmd));
	if (!skb)
		return -ENOMEM;

	cmd = (struct wmi_ap_hidden_ssid_cmd *) skb->data;
	cmd->hidden_ssid = enable ? 1 : 0;

	return ath6kl_wmi_cmd_send(wmi, if_idx, skb, WMI_AP_HIDDEN_SSID_CMDID,
				   NO_SYNC_WMIFLAG);
}