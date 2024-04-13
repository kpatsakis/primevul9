static int ath6kl_wmi_simple_cmd(struct wmi *wmi, u8 if_idx,
				 enum wmi_cmd_id cmd_id)
{
	struct sk_buff *skb;
	int ret;

	skb = ath6kl_wmi_get_new_buf(0);
	if (!skb)
		return -ENOMEM;

	ret = ath6kl_wmi_cmd_send(wmi, if_idx, skb, cmd_id, NO_SYNC_WMIFLAG);

	return ret;
}