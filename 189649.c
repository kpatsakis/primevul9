static void wmi_evt_ready(struct wil6210_priv *wil, int id, void *d, int len)
{
	struct wireless_dev *wdev = wil->wdev;
	struct wmi_ready_event *evt = d;

	wil->n_mids = evt->numof_additional_mids;

	wil_info(wil, "FW ver. %s(SW %d); MAC %pM; %d MID's\n",
		 wil->fw_version, le32_to_cpu(evt->sw_version),
		 evt->mac, wil->n_mids);
	/* ignore MAC address, we already have it from the boot loader */
	strlcpy(wdev->wiphy->fw_version, wil->fw_version,
		sizeof(wdev->wiphy->fw_version));

	if (len > offsetof(struct wmi_ready_event, rfc_read_calib_result)) {
		wil_dbg_wmi(wil, "rfc calibration result %d\n",
			    evt->rfc_read_calib_result);
		wil->fw_calib_result = evt->rfc_read_calib_result;
	}
	wil_set_recovery_state(wil, fw_recovery_idle);
	set_bit(wil_status_fwready, wil->status);
	/* let the reset sequence continue */
	complete(&wil->wmi_ready);
}