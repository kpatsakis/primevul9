static void bond_loadbalance_arp_mon(struct bonding *bond)
{
	struct slave *slave, *oldcurrent;
	struct list_head *iter;
	int do_failover = 0, slave_state_changed = 0;

	if (!bond_has_slaves(bond))
		goto re_arm;

	rcu_read_lock();

	oldcurrent = rcu_dereference(bond->curr_active_slave);
	/* see if any of the previous devices are up now (i.e. they have
	 * xmt and rcv traffic). the curr_active_slave does not come into
	 * the picture unless it is null. also, slave->last_link_up is not
	 * needed here because we send an arp on each slave and give a slave
	 * as long as it needs to get the tx/rx within the delta.
	 * TODO: what about up/down delay in arp mode? it wasn't here before
	 *       so it can wait
	 */
	bond_for_each_slave_rcu(bond, slave, iter) {
		unsigned long trans_start = dev_trans_start(slave->dev);

		bond_propose_link_state(slave, BOND_LINK_NOCHANGE);

		if (slave->link != BOND_LINK_UP) {
			if (bond_time_in_interval(bond, trans_start, 1) &&
			    bond_time_in_interval(bond, slave->last_rx, 1)) {

				bond_propose_link_state(slave, BOND_LINK_UP);
				slave_state_changed = 1;

				/* primary_slave has no meaning in round-robin
				 * mode. the window of a slave being up and
				 * curr_active_slave being null after enslaving
				 * is closed.
				 */
				if (!oldcurrent) {
					slave_info(bond->dev, slave->dev, "link status definitely up\n");
					do_failover = 1;
				} else {
					slave_info(bond->dev, slave->dev, "interface is now up\n");
				}
			}
		} else {
			/* slave->link == BOND_LINK_UP */

			/* not all switches will respond to an arp request
			 * when the source ip is 0, so don't take the link down
			 * if we don't know our ip yet
			 */
			if (!bond_time_in_interval(bond, trans_start, 2) ||
			    !bond_time_in_interval(bond, slave->last_rx, 2)) {

				bond_propose_link_state(slave, BOND_LINK_DOWN);
				slave_state_changed = 1;

				if (slave->link_failure_count < UINT_MAX)
					slave->link_failure_count++;

				slave_info(bond->dev, slave->dev, "interface is now down\n");

				if (slave == oldcurrent)
					do_failover = 1;
			}
		}

		/* note: if switch is in round-robin mode, all links
		 * must tx arp to ensure all links rx an arp - otherwise
		 * links may oscillate or not come up at all; if switch is
		 * in something like xor mode, there is nothing we can
		 * do - all replies will be rx'ed on same link causing slaves
		 * to be unstable during low/no traffic periods
		 */
		if (bond_slave_is_up(slave))
			bond_arp_send_all(bond, slave);
	}

	rcu_read_unlock();

	if (do_failover || slave_state_changed) {
		if (!rtnl_trylock())
			goto re_arm;

		bond_for_each_slave(bond, slave, iter) {
			if (slave->link_new_state != BOND_LINK_NOCHANGE)
				slave->link = slave->link_new_state;
		}

		if (slave_state_changed) {
			bond_slave_state_change(bond);
			if (BOND_MODE(bond) == BOND_MODE_XOR)
				bond_update_slave_arr(bond, NULL);
		}
		if (do_failover) {
			block_netpoll_tx();
			bond_select_active_slave(bond);
			unblock_netpoll_tx();
		}
		rtnl_unlock();
	}

re_arm:
	if (bond->params.arp_interval)
		queue_delayed_work(bond->wq, &bond->arp_work,
				   msecs_to_jiffies(bond->params.arp_interval));
}