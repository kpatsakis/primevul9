static int ath6kl_wmi_keepalive_reply_rx(struct wmi *wmi, u8 *datap, int len)
{
	if (len < sizeof(struct wmi_get_keepalive_cmd))
		return -EINVAL;

	ath6kl_wakeup_event(wmi->parent_dev);

	return 0;
}