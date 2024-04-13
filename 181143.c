int ath6kl_wmi_ap_set_mlme(struct wmi *wmip, u8 if_idx, u8 cmd, const u8 *mac,
			   u16 reason)
{
	struct sk_buff *skb;
	struct wmi_ap_set_mlme_cmd *cm;

	skb = ath6kl_wmi_get_new_buf(sizeof(*cm));
	if (!skb)
		return -ENOMEM;

	cm = (struct wmi_ap_set_mlme_cmd *) skb->data;
	memcpy(cm->mac, mac, ETH_ALEN);
	cm->reason = cpu_to_le16(reason);
	cm->cmd = cmd;

	ath6kl_dbg(ATH6KL_DBG_WMI, "ap_set_mlme: cmd=%d reason=%d\n", cm->cmd,
		   cm->reason);

	return ath6kl_wmi_cmd_send(wmip, if_idx, skb, WMI_AP_SET_MLME_CMDID,
				   NO_SYNC_WMIFLAG);
}