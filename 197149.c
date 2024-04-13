static void bond_do_fail_over_mac(struct bonding *bond,
				  struct slave *new_active,
				  struct slave *old_active)
{
	u8 tmp_mac[MAX_ADDR_LEN];
	struct sockaddr_storage ss;
	int rv;

	switch (bond->params.fail_over_mac) {
	case BOND_FOM_ACTIVE:
		if (new_active) {
			rv = bond_set_dev_addr(bond->dev, new_active->dev);
			if (rv)
				slave_err(bond->dev, new_active->dev, "Error %d setting bond MAC from slave\n",
					  -rv);
		}
		break;
	case BOND_FOM_FOLLOW:
		/* if new_active && old_active, swap them
		 * if just old_active, do nothing (going to no active slave)
		 * if just new_active, set new_active to bond's MAC
		 */
		if (!new_active)
			return;

		if (!old_active)
			old_active = bond_get_old_active(bond, new_active);

		if (old_active) {
			bond_hw_addr_copy(tmp_mac, new_active->dev->dev_addr,
					  new_active->dev->addr_len);
			bond_hw_addr_copy(ss.__data,
					  old_active->dev->dev_addr,
					  old_active->dev->addr_len);
			ss.ss_family = new_active->dev->type;
		} else {
			bond_hw_addr_copy(ss.__data, bond->dev->dev_addr,
					  bond->dev->addr_len);
			ss.ss_family = bond->dev->type;
		}

		rv = dev_set_mac_address(new_active->dev,
					 (struct sockaddr *)&ss, NULL);
		if (rv) {
			slave_err(bond->dev, new_active->dev, "Error %d setting MAC of new active slave\n",
				  -rv);
			goto out;
		}

		if (!old_active)
			goto out;

		bond_hw_addr_copy(ss.__data, tmp_mac,
				  new_active->dev->addr_len);
		ss.ss_family = old_active->dev->type;

		rv = dev_set_mac_address(old_active->dev,
					 (struct sockaddr *)&ss, NULL);
		if (rv)
			slave_err(bond->dev, old_active->dev, "Error %d setting MAC of old active slave\n",
				  -rv);
out:
		break;
	default:
		netdev_err(bond->dev, "bond_do_fail_over_mac impossible: bad policy %d\n",
			   bond->params.fail_over_mac);
		break;
	}

}