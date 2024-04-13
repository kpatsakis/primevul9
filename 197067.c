static void bond_info_query(struct net_device *bond_dev, struct ifbond *info)
{
	struct bonding *bond = netdev_priv(bond_dev);

	bond_fill_ifbond(bond, info);
}