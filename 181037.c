int ath6kl_wmi_ap_set_apsd(struct wmi *wmi, u8 if_idx, u8 enable)
{
	struct wmi_ap_set_apsd_cmd *cmd;
	struct sk_buff *skb;

	skb = ath6kl_wmi_get_new_buf(sizeof(*cmd));
	if (!skb)
		return -ENOMEM;

	cmd = (struct wmi_ap_set_apsd_cmd *)skb->data;
	cmd->enable = enable;

	return ath6kl_wmi_cmd_send(wmi, if_idx, skb, WMI_AP_SET_APSD_CMDID,
				   NO_SYNC_WMIFLAG);
}