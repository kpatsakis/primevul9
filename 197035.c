static struct slave *bond_get_old_active(struct bonding *bond,
					 struct slave *new_active)
{
	struct slave *slave;
	struct list_head *iter;

	bond_for_each_slave(bond, slave, iter) {
		if (slave == new_active)
			continue;

		if (ether_addr_equal(bond->dev->dev_addr, slave->dev->dev_addr))
			return slave;
	}

	return NULL;
}