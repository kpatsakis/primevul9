static void bond_mii_monitor(struct work_struct *work)
{
	struct bonding *bond = container_of(work, struct bonding,
					    mii_work.work);
	bool should_notify_peers = false;
	bool commit;
	unsigned long delay;
	struct slave *slave;
	struct list_head *iter;

	delay = msecs_to_jiffies(bond->params.miimon);

	if (!bond_has_slaves(bond))
		goto re_arm;

	rcu_read_lock();
	should_notify_peers = bond_should_notify_peers(bond);
	commit = !!bond_miimon_inspect(bond);
	if (bond->send_peer_notif) {
		rcu_read_unlock();
		if (rtnl_trylock()) {
			bond->send_peer_notif--;
			rtnl_unlock();
		}
	} else {
		rcu_read_unlock();
	}

	if (commit) {
		/* Race avoidance with bond_close cancel of workqueue */
		if (!rtnl_trylock()) {
			delay = 1;
			should_notify_peers = false;
			goto re_arm;
		}

		bond_for_each_slave(bond, slave, iter) {
			bond_commit_link_state(slave, BOND_SLAVE_NOTIFY_LATER);
		}
		bond_miimon_commit(bond);

		rtnl_unlock();	/* might sleep, hold no other locks */
	}

re_arm:
	if (bond->params.miimon)
		queue_delayed_work(bond->wq, &bond->mii_work, delay);

	if (should_notify_peers) {
		if (!rtnl_trylock())
			return;
		call_netdevice_notifiers(NETDEV_NOTIFY_PEERS, bond->dev);
		rtnl_unlock();
	}
}