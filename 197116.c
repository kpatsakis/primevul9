static struct slave *bond_get_slave_by_id(struct bonding *bond,
					  int slave_id)
{
	struct list_head *iter;
	struct slave *slave;
	int i = slave_id;

	/* Here we start from the slave with slave_id */
	bond_for_each_slave_rcu(bond, slave, iter) {
		if (--i < 0) {
			if (bond_slave_can_tx(slave))
				return slave;
		}
	}

	/* Here we start from the first slave up to slave_id */
	i = slave_id;
	bond_for_each_slave_rcu(bond, slave, iter) {
		if (--i < 0)
			break;
		if (bond_slave_can_tx(slave))
			return slave;
	}
	/* no slave that can tx has been found */
	return NULL;
}