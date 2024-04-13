static int ath6kl_wmi_pspoll_event_rx(struct wmi *wmi, u8 *datap, int len,
				      struct ath6kl_vif *vif)
{
	struct wmi_pspoll_event *ev;

	if (len < sizeof(struct wmi_pspoll_event))
		return -EINVAL;

	ev = (struct wmi_pspoll_event *) datap;

	ath6kl_pspoll_event(vif, le16_to_cpu(ev->aid));

	return 0;
}