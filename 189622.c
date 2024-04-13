int wmi_pcp_stop(struct wil6210_priv *wil)
{
	int rc;

	rc = wmi_led_cfg(wil, false);
	if (rc)
		return rc;

	return wmi_call(wil, WMI_PCP_STOP_CMDID, NULL, 0,
			WMI_PCP_STOPPED_EVENTID, NULL, 0, 20);
}