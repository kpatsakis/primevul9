static int ath6kl_wmi_dtimexpiry_event_rx(struct wmi *wmi, u8 *datap, int len,
					  struct ath6kl_vif *vif)
{
	ath6kl_dtimexpiry_event(vif);

	return 0;
}