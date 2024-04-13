int ath6kl_wmi_set_lpreamble_cmd(struct wmi *wmi, u8 if_idx, u8 status,
				 u8 preamble_policy)
{
	struct sk_buff *skb;
	struct wmi_set_lpreamble_cmd *cmd;
	int ret;

	skb = ath6kl_wmi_get_new_buf(sizeof(struct wmi_set_lpreamble_cmd));
	if (!skb)
		return -ENOMEM;

	cmd = (struct wmi_set_lpreamble_cmd *) skb->data;
	cmd->status = status;
	cmd->preamble_policy = preamble_policy;

	ret = ath6kl_wmi_cmd_send(wmi, if_idx, skb, WMI_SET_LPREAMBLE_CMDID,
				  NO_SYNC_WMIFLAG);
	return ret;
}