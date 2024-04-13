static int igmp_netdev_event(struct notifier_block *this,
			     unsigned long event, void *ptr)
{
	struct net_device *dev = netdev_notifier_info_to_dev(ptr);
	struct in_device *in_dev;

	switch (event) {
	case NETDEV_RESEND_IGMP:
		in_dev = __in_dev_get_rtnl(dev);
		if (in_dev)
			ip_mc_rejoin_groups(in_dev);
		break;
	default:
		break;
	}
	return NOTIFY_DONE;
}