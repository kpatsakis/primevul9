static bool bond_has_this_ip(struct bonding *bond, __be32 ip)
{
	struct netdev_nested_priv priv = {
		.data = (void *)&ip,
	};
	bool ret = false;

	if (ip == bond_confirm_addr(bond->dev, 0, ip))
		return true;

	rcu_read_lock();
	if (netdev_walk_all_upper_dev_rcu(bond->dev, bond_upper_dev_walk, &priv))
		ret = true;
	rcu_read_unlock();

	return ret;
}