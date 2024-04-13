static int ath6kl_wmi_test_rx(struct wmi *wmi, u8 *datap, int len)
{
	ath6kl_tm_rx_event(wmi->parent_dev, datap, len);

	return 0;
}