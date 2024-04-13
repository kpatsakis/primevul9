static int nfc_genl_rcv_nl_event(struct notifier_block *this,
				 unsigned long event, void *ptr)
{
	struct netlink_notify *n = ptr;
	struct urelease_work *w;

	if (event != NETLINK_URELEASE || n->protocol != NETLINK_GENERIC)
		goto out;

	pr_debug("NETLINK_URELEASE event from id %d\n", n->portid);

	w = kmalloc(sizeof(*w), GFP_ATOMIC);
	if (w) {
		INIT_WORK(&w->w, nfc_urelease_event_work);
		w->portid = n->portid;
		schedule_work(&w->w);
	}

out:
	return NOTIFY_DONE;
}