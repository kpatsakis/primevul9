static void rt6_probe_deferred(struct work_struct *w)
{
	struct in6_addr mcaddr;
	struct __rt6_probe_work *work =
		container_of(w, struct __rt6_probe_work, work);

	addrconf_addr_solict_mult(&work->target, &mcaddr);
	ndisc_send_ns(work->dev, NULL, &work->target, &mcaddr, NULL);
	dev_put(work->dev);
	kfree(w);
}