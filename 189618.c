int wmi_get_mgmt_retry(struct wil6210_priv *wil, u8 *retry_short)
{
	int rc;
	struct {
		struct wmi_cmd_hdr wmi;
		struct wmi_get_mgmt_retry_limit_event evt;
	} __packed reply;

	wil_dbg_wmi(wil, "getting mgmt retry short\n");

	if (!test_bit(WMI_FW_CAPABILITY_MGMT_RETRY_LIMIT, wil->fw_capabilities))
		return -ENOTSUPP;

	reply.evt.mgmt_retry_limit = 0;
	rc = wmi_call(wil, WMI_GET_MGMT_RETRY_LIMIT_CMDID, NULL, 0,
		      WMI_GET_MGMT_RETRY_LIMIT_EVENTID, &reply, sizeof(reply),
		      100);
	if (rc)
		return rc;

	if (retry_short)
		*retry_short = reply.evt.mgmt_retry_limit;

	return 0;
}