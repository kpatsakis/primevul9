static int bond_close(struct net_device *bond_dev)
{
	struct bonding *bond = netdev_priv(bond_dev);

	bond_work_cancel_all(bond);
	bond->send_peer_notif = 0;
	if (bond_is_lb(bond))
		bond_alb_deinitialize(bond);
	bond->recv_probe = NULL;

	return 0;
}