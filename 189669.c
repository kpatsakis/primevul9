int wmi_abort_scan(struct wil6210_priv *wil)
{
	int rc;

	wil_dbg_wmi(wil, "sending WMI_ABORT_SCAN_CMDID\n");

	rc = wmi_send(wil, WMI_ABORT_SCAN_CMDID, NULL, 0);
	if (rc)
		wil_err(wil, "Failed to abort scan (%d)\n", rc);

	return rc;
}