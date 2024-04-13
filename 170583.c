void aarp_device_down(struct net_device *dev)
{
	int ct;

	write_lock_bh(&aarp_lock);

	for (ct = 0; ct < AARP_HASH_SIZE; ct++) {
		__aarp_expire_device(&resolved[ct], dev);
		__aarp_expire_device(&unresolved[ct], dev);
		__aarp_expire_device(&proxies[ct], dev);
	}

	write_unlock_bh(&aarp_lock);
}