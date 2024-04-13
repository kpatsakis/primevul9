static void bond_activebackup_arp_mon(struct bonding *bond)
{
	bool should_notify_peers = false;
	bool should_notify_rtnl = false;
	int delta_in_ticks;

	delta_in_ticks = msecs_to_jiffies(bond->params.arp_interval);

	if (!bond_has_slaves(bond))
		goto re_arm;

	rcu_read_lock();

	should_notify_peers = bond_should_notify_peers(bond);

	if (bond_ab_arp_inspect(bond)) {
		rcu_read_unlock();

		/* Race avoidance with bond_close flush of workqueue */
		if (!rtnl_trylock()) {
			delta_in_ticks = 1;
			should_notify_peers = false;
			goto re_arm;
		}

		bond_ab_arp_commit(bond);

		rtnl_unlock();
		rcu_read_lock();
	}

	should_notify_rtnl = bond_ab_arp_probe(bond);
	rcu_read_unlock();

re_arm:
	if (bond->params.arp_interval)
		queue_delayed_work(bond->wq, &bond->arp_work, delta_in_ticks);

	if (should_notify_peers || should_notify_rtnl) {
		if (!rtnl_trylock())
			return;

		if (should_notify_peers)
			call_netdevice_notifiers(NETDEV_NOTIFY_PEERS,
						 bond->dev);
		if (should_notify_rtnl) {
			bond_slave_state_notify(bond);
			bond_slave_link_notify(bond);
		}

		rtnl_unlock();
	}
}