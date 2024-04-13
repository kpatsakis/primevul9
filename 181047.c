static int ath6kl_wmi_send_rssi_threshold_params(struct wmi *wmi,
			struct wmi_rssi_threshold_params_cmd *rssi_cmd)
{
	struct sk_buff *skb;
	struct wmi_rssi_threshold_params_cmd *cmd;

	skb = ath6kl_wmi_get_new_buf(sizeof(*cmd));
	if (!skb)
		return -ENOMEM;

	cmd = (struct wmi_rssi_threshold_params_cmd *) skb->data;
	memcpy(cmd, rssi_cmd, sizeof(struct wmi_rssi_threshold_params_cmd));

	return ath6kl_wmi_cmd_send(wmi, 0, skb, WMI_RSSI_THRESHOLD_PARAMS_CMDID,
				   NO_SYNC_WMIFLAG);
}