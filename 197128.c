static void bond_miimon_commit(struct bonding *bond)
{
	struct list_head *iter;
	struct slave *slave, *primary;

	bond_for_each_slave(bond, slave, iter) {
		switch (slave->link_new_state) {
		case BOND_LINK_NOCHANGE:
			/* For 802.3ad mode, check current slave speed and
			 * duplex again in case its port was disabled after
			 * invalid speed/duplex reporting but recovered before
			 * link monitoring could make a decision on the actual
			 * link status
			 */
			if (BOND_MODE(bond) == BOND_MODE_8023AD &&
			    slave->link == BOND_LINK_UP)
				bond_3ad_adapter_speed_duplex_changed(slave);
			continue;

		case BOND_LINK_UP:
			if (bond_update_speed_duplex(slave) &&
			    bond_needs_speed_duplex(bond)) {
				slave->link = BOND_LINK_DOWN;
				if (net_ratelimit())
					slave_warn(bond->dev, slave->dev,
						   "failed to get link speed/duplex\n");
				continue;
			}
			bond_set_slave_link_state(slave, BOND_LINK_UP,
						  BOND_SLAVE_NOTIFY_NOW);
			slave->last_link_up = jiffies;

			primary = rtnl_dereference(bond->primary_slave);
			if (BOND_MODE(bond) == BOND_MODE_8023AD) {
				/* prevent it from being the active one */
				bond_set_backup_slave(slave);
			} else if (BOND_MODE(bond) != BOND_MODE_ACTIVEBACKUP) {
				/* make it immediately active */
				bond_set_active_slave(slave);
			}

			slave_info(bond->dev, slave->dev, "link status definitely up, %u Mbps %s duplex\n",
				   slave->speed == SPEED_UNKNOWN ? 0 : slave->speed,
				   slave->duplex ? "full" : "half");

			bond_miimon_link_change(bond, slave, BOND_LINK_UP);

			if (!bond->curr_active_slave || slave == primary)
				goto do_failover;

			continue;

		case BOND_LINK_DOWN:
			if (slave->link_failure_count < UINT_MAX)
				slave->link_failure_count++;

			bond_set_slave_link_state(slave, BOND_LINK_DOWN,
						  BOND_SLAVE_NOTIFY_NOW);

			if (BOND_MODE(bond) == BOND_MODE_ACTIVEBACKUP ||
			    BOND_MODE(bond) == BOND_MODE_8023AD)
				bond_set_slave_inactive_flags(slave,
							      BOND_SLAVE_NOTIFY_NOW);

			slave_info(bond->dev, slave->dev, "link status definitely down, disabling slave\n");

			bond_miimon_link_change(bond, slave, BOND_LINK_DOWN);

			if (slave == rcu_access_pointer(bond->curr_active_slave))
				goto do_failover;

			continue;

		default:
			slave_err(bond->dev, slave->dev, "invalid new link %d on slave\n",
				  slave->link_new_state);
			bond_propose_link_state(slave, BOND_LINK_NOCHANGE);

			continue;
		}

do_failover:
		block_netpoll_tx();
		bond_select_active_slave(bond);
		unblock_netpoll_tx();
	}

	bond_set_carrier(bond);
}