static int bond_netdev_event(struct notifier_block *this,
			     unsigned long event, void *ptr)
{
	struct net_device *event_dev = netdev_notifier_info_to_dev(ptr);

	netdev_dbg(event_dev, "%s received %s\n",
		   __func__, netdev_cmd_to_name(event));

	if (!(event_dev->priv_flags & IFF_BONDING))
		return NOTIFY_DONE;

	if (event_dev->flags & IFF_MASTER) {
		int ret;

		ret = bond_master_netdev_event(event, event_dev);
		if (ret != NOTIFY_DONE)
			return ret;
	}

	if (event_dev->flags & IFF_SLAVE)
		return bond_slave_netdev_event(event, event_dev);

	return NOTIFY_DONE;
}