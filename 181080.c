int ath6kl_wmi_test_cmd(struct wmi *wmi, void *buf, size_t len)
{
	struct sk_buff *skb;
	int ret;

	skb = ath6kl_wmi_get_new_buf(len);
	if (!skb)
		return -ENOMEM;

	memcpy(skb->data, buf, len);

	ret = ath6kl_wmi_cmd_send(wmi, 0, skb, WMI_TEST_CMDID, NO_SYNC_WMIFLAG);

	return ret;
}