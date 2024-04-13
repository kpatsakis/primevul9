int wmi_suspend(struct wil6210_priv *wil)
{
	int rc;
	struct wmi_traffic_suspend_cmd cmd = {
		.wakeup_trigger = wil->wakeup_trigger,
	};
	struct {
		struct wmi_cmd_hdr wmi;
		struct wmi_traffic_suspend_event evt;
	} __packed reply;
	u32 suspend_to = WIL_WAIT_FOR_SUSPEND_RESUME_COMP;

	wil->suspend_resp_rcvd = false;
	wil->suspend_resp_comp = false;

	reply.evt.status = WMI_TRAFFIC_SUSPEND_REJECTED;

	rc = wmi_call(wil, WMI_TRAFFIC_SUSPEND_CMDID, &cmd, sizeof(cmd),
		      WMI_TRAFFIC_SUSPEND_EVENTID, &reply, sizeof(reply),
		      suspend_to);
	if (rc) {
		wil_err(wil, "wmi_call for suspend req failed, rc=%d\n", rc);
		if (rc == -ETIME)
			/* wmi_call TO */
			wil->suspend_stats.rejected_by_device++;
		else
			wil->suspend_stats.rejected_by_host++;
		goto out;
	}

	wil_dbg_wmi(wil, "waiting for suspend_response_completed\n");

	rc = wait_event_interruptible_timeout(wil->wq,
					      wil->suspend_resp_comp,
					      msecs_to_jiffies(suspend_to));
	if (rc == 0) {
		wil_err(wil, "TO waiting for suspend_response_completed\n");
		if (wil->suspend_resp_rcvd)
			/* Device responded but we TO due to another reason */
			wil->suspend_stats.rejected_by_host++;
		else
			wil->suspend_stats.rejected_by_device++;
		rc = -EBUSY;
		goto out;
	}

	wil_dbg_wmi(wil, "suspend_response_completed rcvd\n");
	if (reply.evt.status == WMI_TRAFFIC_SUSPEND_REJECTED) {
		wil_dbg_pm(wil, "device rejected the suspend\n");
		wil->suspend_stats.rejected_by_device++;
	}
	rc = reply.evt.status;

out:
	wil->suspend_resp_rcvd = false;
	wil->suspend_resp_comp = false;

	return rc;
}