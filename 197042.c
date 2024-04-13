static bool bond_ab_arp_probe(struct bonding *bond)
{
	struct slave *slave, *before = NULL, *new_slave = NULL,
		     *curr_arp_slave = rcu_dereference(bond->current_arp_slave),
		     *curr_active_slave = rcu_dereference(bond->curr_active_slave);
	struct list_head *iter;
	bool found = false;
	bool should_notify_rtnl = BOND_SLAVE_NOTIFY_LATER;

	if (curr_arp_slave && curr_active_slave)
		netdev_info(bond->dev, "PROBE: c_arp %s && cas %s BAD\n",
			    curr_arp_slave->dev->name,
			    curr_active_slave->dev->name);

	if (curr_active_slave) {
		bond_arp_send_all(bond, curr_active_slave);
		return should_notify_rtnl;
	}

	/* if we don't have a curr_active_slave, search for the next available
	 * backup slave from the current_arp_slave and make it the candidate
	 * for becoming the curr_active_slave
	 */

	if (!curr_arp_slave) {
		curr_arp_slave = bond_first_slave_rcu(bond);
		if (!curr_arp_slave)
			return should_notify_rtnl;
	}

	bond_for_each_slave_rcu(bond, slave, iter) {
		if (!found && !before && bond_slave_is_up(slave))
			before = slave;

		if (found && !new_slave && bond_slave_is_up(slave))
			new_slave = slave;
		/* if the link state is up at this point, we
		 * mark it down - this can happen if we have
		 * simultaneous link failures and
		 * reselect_active_interface doesn't make this
		 * one the current slave so it is still marked
		 * up when it is actually down
		 */
		if (!bond_slave_is_up(slave) && slave->link == BOND_LINK_UP) {
			bond_set_slave_link_state(slave, BOND_LINK_DOWN,
						  BOND_SLAVE_NOTIFY_LATER);
			if (slave->link_failure_count < UINT_MAX)
				slave->link_failure_count++;

			bond_set_slave_inactive_flags(slave,
						      BOND_SLAVE_NOTIFY_LATER);

			slave_info(bond->dev, slave->dev, "backup interface is now down\n");
		}
		if (slave == curr_arp_slave)
			found = true;
	}

	if (!new_slave && before)
		new_slave = before;

	if (!new_slave)
		goto check_state;

	bond_set_slave_link_state(new_slave, BOND_LINK_BACK,
				  BOND_SLAVE_NOTIFY_LATER);
	bond_set_slave_active_flags(new_slave, BOND_SLAVE_NOTIFY_LATER);
	bond_arp_send_all(bond, new_slave);
	new_slave->last_link_up = jiffies;
	rcu_assign_pointer(bond->current_arp_slave, new_slave);

check_state:
	bond_for_each_slave_rcu(bond, slave, iter) {
		if (slave->should_notify || slave->should_notify_link) {
			should_notify_rtnl = BOND_SLAVE_NOTIFY_NOW;
			break;
		}
	}
	return should_notify_rtnl;
}