void wmi_event_worker(struct work_struct *work)
{
	struct wil6210_priv *wil = container_of(work, struct wil6210_priv,
						 wmi_event_worker);
	struct pending_wmi_event *evt;
	struct list_head *lh;

	wil_dbg_wmi(wil, "event_worker: Start\n");
	while ((lh = next_wmi_ev(wil)) != NULL) {
		evt = list_entry(lh, struct pending_wmi_event, list);
		wmi_event_handle(wil, &evt->event.hdr);
		kfree(evt);
	}
	wil_dbg_wmi(wil, "event_worker: Finished\n");
}