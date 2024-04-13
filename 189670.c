int wmi_ps_dev_profile_cfg(struct wil6210_priv *wil,
			   enum wmi_ps_profile_type ps_profile)
{
	int rc;
	struct wmi_ps_dev_profile_cfg_cmd cmd = {
		.ps_profile = ps_profile,
	};
	struct {
		struct wmi_cmd_hdr wmi;
		struct wmi_ps_dev_profile_cfg_event evt;
	} __packed reply;
	u32 status;

	wil_dbg_wmi(wil, "Setting ps dev profile %d\n", ps_profile);

	reply.evt.status = cpu_to_le32(WMI_PS_CFG_CMD_STATUS_ERROR);

	rc = wmi_call(wil, WMI_PS_DEV_PROFILE_CFG_CMDID, &cmd, sizeof(cmd),
		      WMI_PS_DEV_PROFILE_CFG_EVENTID, &reply, sizeof(reply),
		      100);
	if (rc)
		return rc;

	status = le32_to_cpu(reply.evt.status);

	if (status != WMI_PS_CFG_CMD_STATUS_SUCCESS) {
		wil_err(wil, "ps dev profile cfg failed with status %d\n",
			status);
		rc = -EINVAL;
	}

	return rc;
}