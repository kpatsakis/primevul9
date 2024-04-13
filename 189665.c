static void wmi_evt_disconnect(struct wil6210_priv *wil, int id,
			       void *d, int len)
{
	struct wmi_disconnect_event *evt = d;
	u16 reason_code = le16_to_cpu(evt->protocol_reason_status);

	wil_info(wil, "Disconnect %pM reason [proto %d wmi %d]\n",
		 evt->bssid, reason_code, evt->disconnect_reason);

	wil->sinfo_gen++;

	if (test_bit(wil_status_resetting, wil->status) ||
	    !test_bit(wil_status_fwready, wil->status)) {
		wil_err(wil, "status_resetting, cancel disconnect event\n");
		/* no need for cleanup, wil_reset will do that */
		return;
	}

	mutex_lock(&wil->mutex);
	wil6210_disconnect(wil, evt->bssid, reason_code, true);
	mutex_unlock(&wil->mutex);
}