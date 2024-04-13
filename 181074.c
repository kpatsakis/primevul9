int ath6kl_wmi_set_apsd_bfrd_traf(struct wmi *wmi, u8 if_idx,
					     u16 aid, u16 bitmap, u32 flags)
{
	struct wmi_ap_apsd_buffered_traffic_cmd *cmd;
	struct sk_buff *skb;

	skb = ath6kl_wmi_get_new_buf(sizeof(*cmd));
	if (!skb)
		return -ENOMEM;

	cmd = (struct wmi_ap_apsd_buffered_traffic_cmd *)skb->data;
	cmd->aid = cpu_to_le16(aid);
	cmd->bitmap = cpu_to_le16(bitmap);
	cmd->flags = cpu_to_le32(flags);

	return ath6kl_wmi_cmd_send(wmi, if_idx, skb,
				   WMI_AP_APSD_BUFFERED_TRAFFIC_CMDID,
				   NO_SYNC_WMIFLAG);
}