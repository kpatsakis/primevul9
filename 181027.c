static int ath6kl_wmi_stats_event_rx(struct wmi *wmi, u8 *datap, int len,
				     struct ath6kl_vif *vif)
{
	ath6kl_tgt_stats_event(vif, datap, len);

	return 0;
}