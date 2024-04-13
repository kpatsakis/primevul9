isdn_net_open(struct net_device *dev)
{
	int i;
	struct net_device *p;
	struct in_device *in_dev;

	/* moved here from isdn_net_reset, because only the master has an
	   interface associated which is supposed to be started. BTW:
	   we need to call netif_start_queue, not netif_wake_queue here */
	netif_start_queue(dev);

	isdn_net_reset(dev);
	/* Fill in the MAC-level header (not needed, but for compatibility... */
	for (i = 0; i < ETH_ALEN - sizeof(u32); i++)
		dev->dev_addr[i] = 0xfc;
	if ((in_dev = dev->ip_ptr) != NULL) {
		/*
		 *      Any address will do - we take the first
		 */
		struct in_ifaddr *ifa = in_dev->ifa_list;
		if (ifa != NULL)
			memcpy(dev->dev_addr + 2, &ifa->ifa_local, 4);
	}

	/* If this interface has slaves, start them also */
	p = MASTER_TO_SLAVE(dev);
	if (p) {
		while (p) {
			isdn_net_reset(p);
			p = MASTER_TO_SLAVE(p);
		}
	}
	isdn_lock_drivers();
	return 0;
}