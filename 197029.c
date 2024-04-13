static struct slave *bond_find_best_slave(struct bonding *bond)
{
	struct slave *slave, *bestslave = NULL;
	struct list_head *iter;
	int mintime = bond->params.updelay;

	slave = bond_choose_primary_or_current(bond);
	if (slave)
		return slave;

	bond_for_each_slave(bond, slave, iter) {
		if (slave->link == BOND_LINK_UP)
			return slave;
		if (slave->link == BOND_LINK_BACK && bond_slave_is_up(slave) &&
		    slave->delay < mintime) {
			mintime = slave->delay;
			bestslave = slave;
		}
	}

	return bestslave;
}