bool wil_is_wmi_idle(struct wil6210_priv *wil)
{
	ulong flags;
	struct wil6210_mbox_ring *r = &wil->mbox_ctl.rx;
	bool rc = false;

	spin_lock_irqsave(&wil->wmi_ev_lock, flags);

	/* Check if there are pending WMI events in the events queue */
	if (!list_empty(&wil->pending_wmi_ev)) {
		wil_dbg_pm(wil, "Pending WMI events in queue\n");
		goto out;
	}

	/* Check if there is a pending WMI call */
	if (wil->reply_id) {
		wil_dbg_pm(wil, "Pending WMI call\n");
		goto out;
	}

	/* Check if there are pending RX events in mbox */
	r->head = wil_r(wil, RGF_MBOX +
			offsetof(struct wil6210_mbox_ctl, rx.head));
	if (r->tail != r->head)
		wil_dbg_pm(wil, "Pending WMI mbox events\n");
	else
		rc = true;

out:
	spin_unlock_irqrestore(&wil->wmi_ev_lock, flags);
	return rc;
}