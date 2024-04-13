static void wmi_evt_scan_complete(struct wil6210_priv *wil, int id,
				  void *d, int len)
{
	mutex_lock(&wil->p2p_wdev_mutex);
	if (wil->scan_request) {
		struct wmi_scan_complete_event *data = d;
		int status = le32_to_cpu(data->status);
		struct cfg80211_scan_info info = {
			.aborted = ((status != WMI_SCAN_SUCCESS) &&
				(status != WMI_SCAN_ABORT_REJECTED)),
		};

		wil_dbg_wmi(wil, "SCAN_COMPLETE(0x%08x)\n", status);
		wil_dbg_misc(wil, "Complete scan_request 0x%p aborted %d\n",
			     wil->scan_request, info.aborted);
		del_timer_sync(&wil->scan_timer);
		cfg80211_scan_done(wil->scan_request, &info);
		wil->radio_wdev = wil->wdev;
		wil->scan_request = NULL;
		wake_up_interruptible(&wil->wq);
		if (wil->p2p.pending_listen_wdev) {
			wil_dbg_misc(wil, "Scheduling delayed listen\n");
			schedule_work(&wil->p2p.delayed_listen_work);
		}
	} else {
		wil_err(wil, "SCAN_COMPLETE while not scanning\n");
	}
	mutex_unlock(&wil->p2p_wdev_mutex);
}