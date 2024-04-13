int wmi_rxon(struct wil6210_priv *wil, bool on)
{
	int rc;
	struct {
		struct wmi_cmd_hdr wmi;
		struct wmi_listen_started_event evt;
	} __packed reply;

	wil_info(wil, "(%s)\n", on ? "on" : "off");

	if (on) {
		rc = wmi_call(wil, WMI_START_LISTEN_CMDID, NULL, 0,
			      WMI_LISTEN_STARTED_EVENTID,
			      &reply, sizeof(reply), 100);
		if ((rc == 0) && (reply.evt.status != WMI_FW_STATUS_SUCCESS))
			rc = -EINVAL;
	} else {
		rc = wmi_call(wil, WMI_DISCOVERY_STOP_CMDID, NULL, 0,
			      WMI_DISCOVERY_STOPPED_EVENTID, NULL, 0, 20);
	}

	return rc;
}