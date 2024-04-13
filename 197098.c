static bool bond_should_notify_peers(struct bonding *bond)
{
	struct slave *slave;

	rcu_read_lock();
	slave = rcu_dereference(bond->curr_active_slave);
	rcu_read_unlock();

	netdev_dbg(bond->dev, "bond_should_notify_peers: slave %s\n",
		   slave ? slave->dev->name : "NULL");

	if (!slave || !bond->send_peer_notif ||
	    bond->send_peer_notif %
	    max(1, bond->params.peer_notif_delay) != 0 ||
	    !netif_carrier_ok(bond->dev) ||
	    test_bit(__LINK_STATE_LINKWATCH_PENDING, &slave->dev->state))
		return false;

	return true;
}