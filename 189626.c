int wmi_pcp_start(struct wil6210_priv *wil, int bi, u8 wmi_nettype,
		  u8 chan, u8 hidden_ssid, u8 is_go)
{
	int rc;

	struct wmi_pcp_start_cmd cmd = {
		.bcon_interval = cpu_to_le16(bi),
		.network_type = wmi_nettype,
		.disable_sec_offload = 1,
		.channel = chan - 1,
		.pcp_max_assoc_sta = max_assoc_sta,
		.hidden_ssid = hidden_ssid,
		.is_go = is_go,
		.disable_ap_sme = disable_ap_sme,
		.abft_len = wil->abft_len,
	};
	struct {
		struct wmi_cmd_hdr wmi;
		struct wmi_pcp_started_event evt;
	} __packed reply;

	if (!wil->privacy)
		cmd.disable_sec = 1;

	if ((cmd.pcp_max_assoc_sta > WIL6210_MAX_CID) ||
	    (cmd.pcp_max_assoc_sta <= 0)) {
		wil_info(wil,
			 "Requested connection limit %u, valid values are 1 - %d. Setting to %d\n",
			 max_assoc_sta, WIL6210_MAX_CID, WIL6210_MAX_CID);
		cmd.pcp_max_assoc_sta = WIL6210_MAX_CID;
	}

	if (disable_ap_sme &&
	    !test_bit(WMI_FW_CAPABILITY_DISABLE_AP_SME,
		      wil->fw_capabilities)) {
		wil_err(wil, "disable_ap_sme not supported by FW\n");
		return -EOPNOTSUPP;
	}

	/*
	 * Processing time may be huge, in case of secure AP it takes about
	 * 3500ms for FW to start AP
	 */
	rc = wmi_call(wil, WMI_PCP_START_CMDID, &cmd, sizeof(cmd),
		      WMI_PCP_STARTED_EVENTID, &reply, sizeof(reply), 5000);
	if (rc)
		return rc;

	if (reply.evt.status != WMI_FW_STATUS_SUCCESS)
		rc = -EINVAL;

	if (wmi_nettype != WMI_NETTYPE_P2P)
		/* Don't fail due to error in the led configuration */
		wmi_led_cfg(wil, true);

	return rc;
}