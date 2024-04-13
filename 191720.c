int netlink_add_tap(struct netlink_tap *nt)
{
	if (unlikely(nt->dev->type != ARPHRD_NETLINK))
		return -EINVAL;

	spin_lock(&netlink_tap_lock);
	list_add_rcu(&nt->list, &netlink_tap_all);
	spin_unlock(&netlink_tap_lock);

	__module_get(nt->module);

	return 0;
}