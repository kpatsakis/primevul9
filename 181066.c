static int ath6kl_wmi_ch_list_reply_rx(struct wmi *wmi, u8 *datap, int len)
{
	if (len < sizeof(struct wmi_channel_list_reply))
		return -EINVAL;

	ath6kl_wakeup_event(wmi->parent_dev);

	return 0;
}