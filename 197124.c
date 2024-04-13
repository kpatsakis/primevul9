int bond_release(struct net_device *bond_dev, struct net_device *slave_dev)
{
	return __bond_release_one(bond_dev, slave_dev, false, false);
}