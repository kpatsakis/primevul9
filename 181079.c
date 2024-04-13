static int ath6kl_wmi_scan_complete_rx(struct wmi *wmi, u8 *datap, int len,
				       struct ath6kl_vif *vif)
{
	struct wmi_scan_complete_event *ev;

	ev = (struct wmi_scan_complete_event *) datap;

	ath6kl_scan_complete_evt(vif, a_sle32_to_cpu(ev->status));
	wmi->is_probe_ssid = false;

	return 0;
}