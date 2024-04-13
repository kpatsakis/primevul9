int wmi_set_mgmt_retry(struct wil6210_priv *wil, u8 retry_short)
{
	int rc;
	struct wmi_set_mgmt_retry_limit_cmd cmd = {
		.mgmt_retry_limit = retry_short,
	};
	struct {
		struct wmi_cmd_hdr wmi;
		struct wmi_set_mgmt_retry_limit_event evt;
	} __packed reply;

	wil_dbg_wmi(wil, "Setting mgmt retry short %d\n", retry_short);

	if (!test_bit(WMI_FW_CAPABILITY_MGMT_RETRY_LIMIT, wil->fw_capabilities))
		return -ENOTSUPP;

	reply.evt.status = WMI_FW_STATUS_FAILURE;

	rc = wmi_call(wil, WMI_SET_MGMT_RETRY_LIMIT_CMDID, &cmd, sizeof(cmd),
		      WMI_SET_MGMT_RETRY_LIMIT_EVENTID, &reply, sizeof(reply),
		      100);
	if (rc)
		return rc;

	if (reply.evt.status != WMI_FW_STATUS_SUCCESS) {
		wil_err(wil, "set mgmt retry limit failed with status %d\n",
			reply.evt.status);
		rc = -EINVAL;
	}

	return rc;
}