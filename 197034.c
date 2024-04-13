static void bond_get_stats(struct net_device *bond_dev,
			   struct rtnl_link_stats64 *stats)
{
	struct bonding *bond = netdev_priv(bond_dev);
	struct rtnl_link_stats64 temp;
	struct list_head *iter;
	struct slave *slave;
	int nest_level = 0;


	rcu_read_lock();
#ifdef CONFIG_LOCKDEP
	nest_level = bond_get_lowest_level_rcu(bond_dev);
#endif

	spin_lock_nested(&bond->stats_lock, nest_level);
	memcpy(stats, &bond->bond_stats, sizeof(*stats));

	bond_for_each_slave_rcu(bond, slave, iter) {
		const struct rtnl_link_stats64 *new =
			dev_get_stats(slave->dev, &temp);

		bond_fold_stats(stats, new, &slave->slave_stats);

		/* save off the slave stats for the next run */
		memcpy(&slave->slave_stats, new, sizeof(*new));
	}

	memcpy(&bond->bond_stats, stats, sizeof(*stats));
	spin_unlock(&bond->stats_lock);
	rcu_read_unlock();
}