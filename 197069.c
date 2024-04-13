static int bond_master_netdev_event(unsigned long event,
				    struct net_device *bond_dev)
{
	struct bonding *event_bond = netdev_priv(bond_dev);

	netdev_dbg(bond_dev, "%s called\n", __func__);

	switch (event) {
	case NETDEV_CHANGENAME:
		return bond_event_changename(event_bond);
	case NETDEV_UNREGISTER:
		bond_remove_proc_entry(event_bond);
		break;
	case NETDEV_REGISTER:
		bond_create_proc_entry(event_bond);
		break;
	default:
		break;
	}

	return NOTIFY_DONE;
}