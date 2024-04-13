static int aarp_device_event(struct notifier_block *this, unsigned long event,
			     void *ptr)
{
	struct net_device *dev = netdev_notifier_info_to_dev(ptr);
	int ct;

	if (!net_eq(dev_net(dev), &init_net))
		return NOTIFY_DONE;

	if (event == NETDEV_DOWN) {
		write_lock_bh(&aarp_lock);

		for (ct = 0; ct < AARP_HASH_SIZE; ct++) {
			__aarp_expire_device(&resolved[ct], dev);
			__aarp_expire_device(&unresolved[ct], dev);
			__aarp_expire_device(&proxies[ct], dev);
		}

		write_unlock_bh(&aarp_lock);
	}
	return NOTIFY_DONE;
}