int wmi_addba_rx_resp(struct wil6210_priv *wil, u8 cid, u8 tid, u8 token,
		      u16 status, bool amsdu, u16 agg_wsize, u16 timeout)
{
	int rc;
	struct wmi_rcp_addba_resp_cmd cmd = {
		.cidxtid = mk_cidxtid(cid, tid),
		.dialog_token = token,
		.status_code = cpu_to_le16(status),
		/* bit 0: A-MSDU supported
		 * bit 1: policy (should be 0 for us)
		 * bits 2..5: TID
		 * bits 6..15: buffer size
		 */
		.ba_param_set = cpu_to_le16((amsdu ? 1 : 0) | (tid << 2) |
					    (agg_wsize << 6)),
		.ba_timeout = cpu_to_le16(timeout),
	};
	struct {
		struct wmi_cmd_hdr wmi;
		struct wmi_rcp_addba_resp_sent_event evt;
	} __packed reply;

	wil_dbg_wmi(wil,
		    "ADDBA response for CID %d TID %d size %d timeout %d status %d AMSDU%s\n",
		    cid, tid, agg_wsize, timeout, status, amsdu ? "+" : "-");

	rc = wmi_call(wil, WMI_RCP_ADDBA_RESP_CMDID, &cmd, sizeof(cmd),
		      WMI_RCP_ADDBA_RESP_SENT_EVENTID, &reply, sizeof(reply),
		      100);
	if (rc)
		return rc;

	if (reply.evt.status) {
		wil_err(wil, "ADDBA response failed with status %d\n",
			le16_to_cpu(reply.evt.status));
		rc = -EINVAL;
	}

	return rc;
}