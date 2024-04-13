int wmi_resume(struct wil6210_priv *wil)
{
	int rc;
	struct {
		struct wmi_cmd_hdr wmi;
		struct wmi_traffic_resume_event evt;
	} __packed reply;

	reply.evt.status = WMI_TRAFFIC_RESUME_FAILED;

	rc = wmi_call(wil, WMI_TRAFFIC_RESUME_CMDID, NULL, 0,
		      WMI_TRAFFIC_RESUME_EVENTID, &reply, sizeof(reply),
		      WIL_WAIT_FOR_SUSPEND_RESUME_COMP);
	if (rc)
		return rc;

	return reply.evt.status;
}