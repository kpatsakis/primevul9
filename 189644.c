int wmi_start_search(struct wil6210_priv *wil)
{
	int rc;
	struct {
		struct wmi_cmd_hdr wmi;
		struct wmi_search_started_event evt;
	} __packed reply;

	wil_dbg_wmi(wil, "sending WMI_START_SEARCH_CMDID\n");

	rc = wmi_call(wil, WMI_START_SEARCH_CMDID, NULL, 0,
		      WMI_SEARCH_STARTED_EVENTID, &reply, sizeof(reply), 300);
	if (!rc && reply.evt.status != WMI_FW_STATUS_SUCCESS) {
		wil_err(wil, "device failed to start search. status %d\n",
			reply.evt.status);
		rc = -EINVAL;
	}

	return rc;
}