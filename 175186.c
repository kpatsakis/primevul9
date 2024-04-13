nfqnl_rcv_dev_event(struct notifier_block *this,
		    unsigned long event, void *ptr)
{
	struct net_device *dev = netdev_notifier_info_to_dev(ptr);

	/* Drop any packets associated with the downed device */
	if (event == NETDEV_DOWN)
		nfqnl_dev_drop(dev_net(dev), dev->ifindex);
	return NOTIFY_DONE;
}