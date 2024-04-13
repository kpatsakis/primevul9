int bond_update_slave_arr(struct bonding *bond, struct slave *skipslave)
{
	struct bond_up_slave *usable_slaves = NULL, *all_slaves = NULL;
	struct slave *slave;
	struct list_head *iter;
	int agg_id = 0;
	int ret = 0;

	might_sleep();

	usable_slaves = kzalloc(struct_size(usable_slaves, arr,
					    bond->slave_cnt), GFP_KERNEL);
	all_slaves = kzalloc(struct_size(all_slaves, arr,
					 bond->slave_cnt), GFP_KERNEL);
	if (!usable_slaves || !all_slaves) {
		ret = -ENOMEM;
		goto out;
	}
	if (BOND_MODE(bond) == BOND_MODE_8023AD) {
		struct ad_info ad_info;

		spin_lock_bh(&bond->mode_lock);
		if (bond_3ad_get_active_agg_info(bond, &ad_info)) {
			spin_unlock_bh(&bond->mode_lock);
			pr_debug("bond_3ad_get_active_agg_info failed\n");
			/* No active aggragator means it's not safe to use
			 * the previous array.
			 */
			bond_reset_slave_arr(bond);
			goto out;
		}
		spin_unlock_bh(&bond->mode_lock);
		agg_id = ad_info.aggregator_id;
	}
	bond_for_each_slave(bond, slave, iter) {
		if (skipslave == slave)
			continue;

		all_slaves->arr[all_slaves->count++] = slave;
		if (BOND_MODE(bond) == BOND_MODE_8023AD) {
			struct aggregator *agg;

			agg = SLAVE_AD_INFO(slave)->port.aggregator;
			if (!agg || agg->aggregator_identifier != agg_id)
				continue;
		}
		if (!bond_slave_can_tx(slave))
			continue;

		slave_dbg(bond->dev, slave->dev, "Adding slave to tx hash array[%d]\n",
			  usable_slaves->count);

		usable_slaves->arr[usable_slaves->count++] = slave;
	}

	bond_set_slave_arr(bond, usable_slaves, all_slaves);
	return ret;
out:
	if (ret != 0 && skipslave) {
		bond_skip_slave(rtnl_dereference(bond->all_slaves),
				skipslave);
		bond_skip_slave(rtnl_dereference(bond->usable_slaves),
				skipslave);
	}
	kfree_rcu(all_slaves, rcu);
	kfree_rcu(usable_slaves, rcu);

	return ret;
}