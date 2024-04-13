int ath6kl_wmi_set_rssi_filter_cmd(struct wmi *wmi, u8 if_idx, s8 rssi)
{
	struct sk_buff *skb;
	struct wmi_set_rssi_filter_cmd *cmd;
	int ret;

	skb = ath6kl_wmi_get_new_buf(sizeof(*cmd));
	if (!skb)
		return -ENOMEM;

	cmd = (struct wmi_set_rssi_filter_cmd *) skb->data;
	cmd->rssi = rssi;

	ret = ath6kl_wmi_cmd_send(wmi, if_idx, skb, WMI_SET_RSSI_FILTER_CMDID,
				  NO_SYNC_WMIFLAG);
	return ret;
}