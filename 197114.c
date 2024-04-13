static int bond_vlan_rx_kill_vid(struct net_device *bond_dev,
				 __be16 proto, u16 vid)
{
	struct bonding *bond = netdev_priv(bond_dev);
	struct list_head *iter;
	struct slave *slave;

	bond_for_each_slave(bond, slave, iter)
		vlan_vid_del(slave->dev, proto, vid);

	if (bond_is_lb(bond))
		bond_alb_clear_vlan(bond, vid);

	return 0;
}