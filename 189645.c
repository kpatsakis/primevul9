void wmi_event_flush(struct wil6210_priv *wil)
{
	ulong flags;
	struct pending_wmi_event *evt, *t;

	wil_dbg_wmi(wil, "event_flush\n");

	spin_lock_irqsave(&wil->wmi_ev_lock, flags);

	list_for_each_entry_safe(evt, t, &wil->pending_wmi_ev, list) {
		list_del(&evt->list);
		kfree(evt);
	}

	spin_unlock_irqrestore(&wil->wmi_ev_lock, flags);
}