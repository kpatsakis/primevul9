static struct list_head *next_wmi_ev(struct wil6210_priv *wil)
{
	ulong flags;
	struct list_head *ret = NULL;

	spin_lock_irqsave(&wil->wmi_ev_lock, flags);

	if (!list_empty(&wil->pending_wmi_ev)) {
		ret = wil->pending_wmi_ev.next;
		list_del(ret);
	}

	spin_unlock_irqrestore(&wil->wmi_ev_lock, flags);

	return ret;
}