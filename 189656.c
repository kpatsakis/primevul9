static void wmi_evt_vring_en(struct wil6210_priv *wil, int id, void *d, int len)
{
	struct wmi_vring_en_event *evt = d;
	u8 vri = evt->vring_index;
	struct wireless_dev *wdev = wil_to_wdev(wil);

	wil_dbg_wmi(wil, "Enable vring %d\n", vri);

	if (vri >= ARRAY_SIZE(wil->vring_tx)) {
		wil_err(wil, "Enable for invalid vring %d\n", vri);
		return;
	}

	if (wdev->iftype != NL80211_IFTYPE_AP || !disable_ap_sme)
		/* in AP mode with disable_ap_sme, this is done by
		 * wil_cfg80211_change_station()
		 */
		wil->vring_tx_data[vri].dot1x_open = true;
	if (vri == wil->bcast_vring) /* no BA for bcast */
		return;
	if (agg_wsize >= 0)
		wil_addba_tx_request(wil, vri, agg_wsize);
}