isdn_net_swapbind(int drvidx)
{
	isdn_net_dev *p;

#ifdef ISDN_DEBUG_NET_ICALL
	printk(KERN_DEBUG "n_fi: swapping ch of %d\n", drvidx);
#endif
	p = dev->netdev;
	while (p) {
		if (p->local->pre_device == drvidx)
			switch (p->local->pre_channel) {
			case 0:
				p->local->pre_channel = 1;
				break;
			case 1:
				p->local->pre_channel = 0;
				break;
			}
		p = (isdn_net_dev *) p->next;
	}
}