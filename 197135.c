static int bond_set_mac_address(struct net_device *bond_dev, void *addr)
{
	struct bonding *bond = netdev_priv(bond_dev);
	struct slave *slave, *rollback_slave;
	struct sockaddr_storage *ss = addr, tmp_ss;
	struct list_head *iter;
	int res = 0;

	if (BOND_MODE(bond) == BOND_MODE_ALB)
		return bond_alb_set_mac_address(bond_dev, addr);


	netdev_dbg(bond_dev, "%s: bond=%p\n", __func__, bond);

	/* If fail_over_mac is enabled, do nothing and return success.
	 * Returning an error causes ifenslave to fail.
	 */
	if (bond->params.fail_over_mac &&
	    BOND_MODE(bond) == BOND_MODE_ACTIVEBACKUP)
		return 0;

	if (!is_valid_ether_addr(ss->__data))
		return -EADDRNOTAVAIL;

	bond_for_each_slave(bond, slave, iter) {
		slave_dbg(bond_dev, slave->dev, "%s: slave=%p\n",
			  __func__, slave);
		res = dev_set_mac_address(slave->dev, addr, NULL);
		if (res) {
			/* TODO: consider downing the slave
			 * and retry ?
			 * User should expect communications
			 * breakage anyway until ARP finish
			 * updating, so...
			 */
			slave_dbg(bond_dev, slave->dev, "%s: err %d\n",
				  __func__, res);
			goto unwind;
		}
	}

	/* success */
	memcpy(bond_dev->dev_addr, ss->__data, bond_dev->addr_len);
	return 0;

unwind:
	memcpy(tmp_ss.__data, bond_dev->dev_addr, bond_dev->addr_len);
	tmp_ss.ss_family = bond_dev->type;

	/* unwind from head to the slave that failed */
	bond_for_each_slave(bond, rollback_slave, iter) {
		int tmp_res;

		if (rollback_slave == slave)
			break;

		tmp_res = dev_set_mac_address(rollback_slave->dev,
					      (struct sockaddr *)&tmp_ss, NULL);
		if (tmp_res) {
			slave_dbg(bond_dev, rollback_slave->dev, "%s: unwind err %d\n",
				   __func__, tmp_res);
		}
	}

	return res;
}