static int bond_miimon_inspect(struct bonding *bond)
{
	int link_state, commit = 0;
	struct list_head *iter;
	struct slave *slave;
	bool ignore_updelay;

	ignore_updelay = !rcu_dereference(bond->curr_active_slave);

	bond_for_each_slave_rcu(bond, slave, iter) {
		bond_propose_link_state(slave, BOND_LINK_NOCHANGE);

		link_state = bond_check_dev_link(bond, slave->dev, 0);

		switch (slave->link) {
		case BOND_LINK_UP:
			if (link_state)
				continue;

			bond_propose_link_state(slave, BOND_LINK_FAIL);
			commit++;
			slave->delay = bond->params.downdelay;
			if (slave->delay) {
				slave_info(bond->dev, slave->dev, "link status down for %sinterface, disabling it in %d ms\n",
					   (BOND_MODE(bond) ==
					    BOND_MODE_ACTIVEBACKUP) ?
					    (bond_is_active_slave(slave) ?
					     "active " : "backup ") : "",
					   bond->params.downdelay * bond->params.miimon);
			}
			fallthrough;
		case BOND_LINK_FAIL:
			if (link_state) {
				/* recovered before downdelay expired */
				bond_propose_link_state(slave, BOND_LINK_UP);
				slave->last_link_up = jiffies;
				slave_info(bond->dev, slave->dev, "link status up again after %d ms\n",
					   (bond->params.downdelay - slave->delay) *
					   bond->params.miimon);
				commit++;
				continue;
			}

			if (slave->delay <= 0) {
				bond_propose_link_state(slave, BOND_LINK_DOWN);
				commit++;
				continue;
			}

			slave->delay--;
			break;

		case BOND_LINK_DOWN:
			if (!link_state)
				continue;

			bond_propose_link_state(slave, BOND_LINK_BACK);
			commit++;
			slave->delay = bond->params.updelay;

			if (slave->delay) {
				slave_info(bond->dev, slave->dev, "link status up, enabling it in %d ms\n",
					   ignore_updelay ? 0 :
					   bond->params.updelay *
					   bond->params.miimon);
			}
			fallthrough;
		case BOND_LINK_BACK:
			if (!link_state) {
				bond_propose_link_state(slave, BOND_LINK_DOWN);
				slave_info(bond->dev, slave->dev, "link status down again after %d ms\n",
					   (bond->params.updelay - slave->delay) *
					   bond->params.miimon);
				commit++;
				continue;
			}

			if (ignore_updelay)
				slave->delay = 0;

			if (slave->delay <= 0) {
				bond_propose_link_state(slave, BOND_LINK_UP);
				commit++;
				ignore_updelay = false;
				continue;
			}

			slave->delay--;
			break;
		}
	}

	return commit;
}