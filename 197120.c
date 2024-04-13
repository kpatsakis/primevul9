static int bond_change_mtu(struct net_device *bond_dev, int new_mtu)
{
	struct bonding *bond = netdev_priv(bond_dev);
	struct slave *slave, *rollback_slave;
	struct list_head *iter;
	int res = 0;

	netdev_dbg(bond_dev, "bond=%p, new_mtu=%d\n", bond, new_mtu);

	bond_for_each_slave(bond, slave, iter) {
		slave_dbg(bond_dev, slave->dev, "s %p c_m %p\n",
			   slave, slave->dev->netdev_ops->ndo_change_mtu);

		res = dev_set_mtu(slave->dev, new_mtu);

		if (res) {
			/* If we failed to set the slave's mtu to the new value
			 * we must abort the operation even in ACTIVE_BACKUP
			 * mode, because if we allow the backup slaves to have
			 * different mtu values than the active slave we'll
			 * need to change their mtu when doing a failover. That
			 * means changing their mtu from timer context, which
			 * is probably not a good idea.
			 */
			slave_dbg(bond_dev, slave->dev, "err %d setting mtu to %d\n",
				  res, new_mtu);
			goto unwind;
		}
	}

	bond_dev->mtu = new_mtu;

	return 0;

unwind:
	/* unwind from head to the slave that failed */
	bond_for_each_slave(bond, rollback_slave, iter) {
		int tmp_res;

		if (rollback_slave == slave)
			break;

		tmp_res = dev_set_mtu(rollback_slave->dev, bond_dev->mtu);
		if (tmp_res)
			slave_dbg(bond_dev, rollback_slave->dev, "unwind err %d\n",
				  tmp_res);
	}

	return res;
}