int ath6kl_wmi_set_rts_cmd(struct wmi *wmi, u16 threshold)
{
	struct sk_buff *skb;
	struct wmi_set_rts_cmd *cmd;
	int ret;

	skb = ath6kl_wmi_get_new_buf(sizeof(struct wmi_set_rts_cmd));
	if (!skb)
		return -ENOMEM;

	cmd = (struct wmi_set_rts_cmd *) skb->data;
	cmd->threshold = cpu_to_le16(threshold);

	ret = ath6kl_wmi_cmd_send(wmi, 0, skb, WMI_SET_RTS_CMDID,
				  NO_SYNC_WMIFLAG);
	return ret;
}