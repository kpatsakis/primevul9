int wmi_call(struct wil6210_priv *wil, u16 cmdid, void *buf, u16 len,
	     u16 reply_id, void *reply, u8 reply_size, int to_msec)
{
	int rc;
	unsigned long remain;

	mutex_lock(&wil->wmi_mutex);

	spin_lock(&wil->wmi_ev_lock);
	wil->reply_id = reply_id;
	wil->reply_buf = reply;
	wil->reply_size = reply_size;
	reinit_completion(&wil->wmi_call);
	spin_unlock(&wil->wmi_ev_lock);

	rc = __wmi_send(wil, cmdid, buf, len);
	if (rc)
		goto out;

	remain = wait_for_completion_timeout(&wil->wmi_call,
					     msecs_to_jiffies(to_msec));
	if (0 == remain) {
		wil_err(wil, "wmi_call(0x%04x->0x%04x) timeout %d msec\n",
			cmdid, reply_id, to_msec);
		rc = -ETIME;
	} else {
		wil_dbg_wmi(wil,
			    "wmi_call(0x%04x->0x%04x) completed in %d msec\n",
			    cmdid, reply_id,
			    to_msec - jiffies_to_msecs(remain));
	}

out:
	spin_lock(&wil->wmi_ev_lock);
	wil->reply_id = 0;
	wil->reply_buf = NULL;
	wil->reply_size = 0;
	spin_unlock(&wil->wmi_ev_lock);

	mutex_unlock(&wil->wmi_mutex);

	return rc;
}