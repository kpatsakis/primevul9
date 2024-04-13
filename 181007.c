int ath6kl_wmi_reconnect_cmd(struct wmi *wmi, u8 if_idx, u8 *bssid,
			     u16 channel)
{
	struct sk_buff *skb;
	struct wmi_reconnect_cmd *cc;
	int ret;

	ath6kl_dbg(ATH6KL_DBG_WMI, "wmi reconnect bssid %pM freq %d\n",
		   bssid, channel);

	wmi->traffic_class = 100;

	skb = ath6kl_wmi_get_new_buf(sizeof(struct wmi_reconnect_cmd));
	if (!skb)
		return -ENOMEM;

	cc = (struct wmi_reconnect_cmd *) skb->data;
	cc->channel = cpu_to_le16(channel);

	if (bssid != NULL)
		memcpy(cc->bssid, bssid, ETH_ALEN);

	ret = ath6kl_wmi_cmd_send(wmi, if_idx, skb, WMI_RECONNECT_CMDID,
				  NO_SYNC_WMIFLAG);

	return ret;
}