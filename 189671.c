int wmi_start_listen(struct wil6210_priv *wil)
{
	int rc;
	struct {
		struct wmi_cmd_hdr wmi;
		struct wmi_listen_started_event evt;
	} __packed reply;

	wil_dbg_wmi(wil, "sending WMI_START_LISTEN_CMDID\n");

	rc = wmi_call(wil, WMI_START_LISTEN_CMDID, NULL, 0,
		      WMI_LISTEN_STARTED_EVENTID, &reply, sizeof(reply), 300);
	if (!rc && reply.evt.status != WMI_FW_STATUS_SUCCESS) {
		wil_err(wil, "device failed to start listen. status %d\n",
			reply.evt.status);
		rc = -EINVAL;
	}

	return rc;
}