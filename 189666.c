int wmi_p2p_cfg(struct wil6210_priv *wil, int channel, int bi)
{
	int rc;
	struct wmi_p2p_cfg_cmd cmd = {
		.discovery_mode = WMI_DISCOVERY_MODE_PEER2PEER,
		.bcon_interval = cpu_to_le16(bi),
		.channel = channel - 1,
	};
	struct {
		struct wmi_cmd_hdr wmi;
		struct wmi_p2p_cfg_done_event evt;
	} __packed reply;

	wil_dbg_wmi(wil, "sending WMI_P2P_CFG_CMDID\n");

	rc = wmi_call(wil, WMI_P2P_CFG_CMDID, &cmd, sizeof(cmd),
		      WMI_P2P_CFG_DONE_EVENTID, &reply, sizeof(reply), 300);
	if (!rc && reply.evt.status != WMI_FW_STATUS_SUCCESS) {
		wil_err(wil, "P2P_CFG failed. status %d\n", reply.evt.status);
		rc = -EINVAL;
	}

	return rc;
}