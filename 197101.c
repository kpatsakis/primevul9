static int bond_ab_arp_inspect(struct bonding *bond)
{
	unsigned long trans_start, last_rx;
	struct list_head *iter;
	struct slave *slave;
	int commit = 0;

	bond_for_each_slave_rcu(bond, slave, iter) {
		bond_propose_link_state(slave, BOND_LINK_NOCHANGE);
		last_rx = slave_last_rx(bond, slave);

		if (slave->link != BOND_LINK_UP) {
			if (bond_time_in_interval(bond, last_rx, 1)) {
				bond_propose_link_state(slave, BOND_LINK_UP);
				commit++;
			} else if (slave->link == BOND_LINK_BACK) {
				bond_propose_link_state(slave, BOND_LINK_FAIL);
				commit++;
			}
			continue;
		}

		/* Give slaves 2*delta after being enslaved or made
		 * active.  This avoids bouncing, as the last receive
		 * times need a full ARP monitor cycle to be updated.
		 */
		if (bond_time_in_interval(bond, slave->last_link_up, 2))
			continue;

		/* Backup slave is down if:
		 * - No current_arp_slave AND
		 * - more than 3*delta since last receive AND
		 * - the bond has an IP address
		 *
		 * Note: a non-null current_arp_slave indicates
		 * the curr_active_slave went down and we are
		 * searching for a new one; under this condition
		 * we only take the curr_active_slave down - this
		 * gives each slave a chance to tx/rx traffic
		 * before being taken out
		 */
		if (!bond_is_active_slave(slave) &&
		    !rcu_access_pointer(bond->current_arp_slave) &&
		    !bond_time_in_interval(bond, last_rx, 3)) {
			bond_propose_link_state(slave, BOND_LINK_DOWN);
			commit++;
		}

		/* Active slave is down if:
		 * - more than 2*delta since transmitting OR
		 * - (more than 2*delta since receive AND
		 *    the bond has an IP address)
		 */
		trans_start = dev_trans_start(slave->dev);
		if (bond_is_active_slave(slave) &&
		    (!bond_time_in_interval(bond, trans_start, 2) ||
		     !bond_time_in_interval(bond, last_rx, 2))) {
			bond_propose_link_state(slave, BOND_LINK_DOWN);
			commit++;
		}
	}

	return commit;
}