static void bond_ab_arp_commit(struct bonding *bond)
{
	unsigned long trans_start;
	struct list_head *iter;
	struct slave *slave;

	bond_for_each_slave(bond, slave, iter) {
		switch (slave->link_new_state) {
		case BOND_LINK_NOCHANGE:
			continue;

		case BOND_LINK_UP:
			trans_start = dev_trans_start(slave->dev);
			if (rtnl_dereference(bond->curr_active_slave) != slave ||
			    (!rtnl_dereference(bond->curr_active_slave) &&
			     bond_time_in_interval(bond, trans_start, 1))) {
				struct slave *current_arp_slave;

				current_arp_slave = rtnl_dereference(bond->current_arp_slave);
				bond_set_slave_link_state(slave, BOND_LINK_UP,
							  BOND_SLAVE_NOTIFY_NOW);
				if (current_arp_slave) {
					bond_set_slave_inactive_flags(
						current_arp_slave,
						BOND_SLAVE_NOTIFY_NOW);
					RCU_INIT_POINTER(bond->current_arp_slave, NULL);
				}

				slave_info(bond->dev, slave->dev, "link status definitely up\n");

				if (!rtnl_dereference(bond->curr_active_slave) ||
				    slave == rtnl_dereference(bond->primary_slave))
					goto do_failover;

			}

			continue;

		case BOND_LINK_DOWN:
			if (slave->link_failure_count < UINT_MAX)
				slave->link_failure_count++;

			bond_set_slave_link_state(slave, BOND_LINK_DOWN,
						  BOND_SLAVE_NOTIFY_NOW);
			bond_set_slave_inactive_flags(slave,
						      BOND_SLAVE_NOTIFY_NOW);

			slave_info(bond->dev, slave->dev, "link status definitely down, disabling slave\n");

			if (slave == rtnl_dereference(bond->curr_active_slave)) {
				RCU_INIT_POINTER(bond->current_arp_slave, NULL);
				goto do_failover;
			}

			continue;

		case BOND_LINK_FAIL:
			bond_set_slave_link_state(slave, BOND_LINK_FAIL,
						  BOND_SLAVE_NOTIFY_NOW);
			bond_set_slave_inactive_flags(slave,
						      BOND_SLAVE_NOTIFY_NOW);

			/* A slave has just been enslaved and has become
			 * the current active slave.
			 */
			if (rtnl_dereference(bond->curr_active_slave))
				RCU_INIT_POINTER(bond->current_arp_slave, NULL);
			continue;

		default:
			slave_err(bond->dev, slave->dev,
				  "impossible: link_new_state %d on slave\n",
				  slave->link_new_state);
			continue;
		}

do_failover:
		block_netpoll_tx();
		bond_select_active_slave(bond);
		unblock_netpoll_tx();
	}

	bond_set_carrier(bond);
}