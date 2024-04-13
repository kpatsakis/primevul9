static void bond_upper_dev_unlink(struct bonding *bond, struct slave *slave)
{
	netdev_upper_dev_unlink(slave->dev, bond->dev);
	slave->dev->flags &= ~IFF_SLAVE;
}