int wmi_stop_discovery(struct wil6210_priv *wil)
{
	int rc;

	wil_dbg_wmi(wil, "sending WMI_DISCOVERY_STOP_CMDID\n");

	rc = wmi_call(wil, WMI_DISCOVERY_STOP_CMDID, NULL, 0,
		      WMI_DISCOVERY_STOPPED_EVENTID, NULL, 0, 100);

	if (rc)
		wil_err(wil, "Failed to stop discovery\n");

	return rc;
}