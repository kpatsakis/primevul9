static int ath6kl_wmi_rx_probe_req_event_rx(struct wmi *wmi, u8 *datap, int len,
					    struct ath6kl_vif *vif)
{
	struct wmi_p2p_rx_probe_req_event *ev;
	u32 freq;
	u16 dlen;

	if (len < sizeof(*ev))
		return -EINVAL;

	ev = (struct wmi_p2p_rx_probe_req_event *) datap;
	freq = le32_to_cpu(ev->freq);
	dlen = le16_to_cpu(ev->len);
	if (datap + len < ev->data + dlen) {
		ath6kl_err("invalid wmi_p2p_rx_probe_req_event: len=%d dlen=%u\n",
			   len, dlen);
		return -EINVAL;
	}
	ath6kl_dbg(ATH6KL_DBG_WMI,
		   "rx_probe_req: len=%u freq=%u probe_req_report=%d\n",
		   dlen, freq, vif->probe_req_report);

	if (vif->probe_req_report || vif->nw_type == AP_NETWORK)
		cfg80211_rx_mgmt(&vif->wdev, freq, 0, ev->data, dlen, 0);

	return 0;
}