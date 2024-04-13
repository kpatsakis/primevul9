static int ath6kl_wmi_neighbor_report_event_rx(struct wmi *wmi, u8 *datap,
					       int len, struct ath6kl_vif *vif)
{
	struct wmi_neighbor_report_event *ev;
	u8 i;

	if (len < sizeof(*ev))
		return -EINVAL;
	ev = (struct wmi_neighbor_report_event *) datap;
	if (struct_size(ev, neighbor, ev->num_neighbors) > len) {
		ath6kl_dbg(ATH6KL_DBG_WMI,
			   "truncated neighbor event (num=%d len=%d)\n",
			   ev->num_neighbors, len);
		return -EINVAL;
	}
	for (i = 0; i < ev->num_neighbors; i++) {
		ath6kl_dbg(ATH6KL_DBG_WMI, "neighbor %d/%d - %pM 0x%x\n",
			   i + 1, ev->num_neighbors, ev->neighbor[i].bssid,
			   ev->neighbor[i].bss_flags);
		cfg80211_pmksa_candidate_notify(vif->ndev, i,
						ev->neighbor[i].bssid,
						!!(ev->neighbor[i].bss_flags &
						   WMI_PREAUTH_CAPABLE_BSS),
						GFP_ATOMIC);
	}

	return 0;
}