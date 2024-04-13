static int ath6kl_wmi_tkip_micerr_event_rx(struct wmi *wmi, u8 *datap, int len,
					   struct ath6kl_vif *vif)
{
	struct wmi_tkip_micerr_event *ev;

	if (len < sizeof(struct wmi_tkip_micerr_event))
		return -EINVAL;

	ev = (struct wmi_tkip_micerr_event *) datap;

	ath6kl_tkip_micerr_event(vif, ev->key_id, ev->is_mcast);

	return 0;
}