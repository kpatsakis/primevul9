int bond_enslave(struct net_device *bond_dev, struct net_device *slave_dev,
		 struct netlink_ext_ack *extack)
{
	struct bonding *bond = netdev_priv(bond_dev);
	const struct net_device_ops *slave_ops = slave_dev->netdev_ops;
	struct slave *new_slave = NULL, *prev_slave;
	struct sockaddr_storage ss;
	int link_reporting;
	int res = 0, i;

	if (slave_dev->flags & IFF_MASTER &&
	    !netif_is_bond_master(slave_dev)) {
		NL_SET_ERR_MSG(extack, "Device with IFF_MASTER cannot be enslaved");
		netdev_err(bond_dev,
			   "Error: Device with IFF_MASTER cannot be enslaved\n");
		return -EPERM;
	}

	if (!bond->params.use_carrier &&
	    slave_dev->ethtool_ops->get_link == NULL &&
	    slave_ops->ndo_do_ioctl == NULL) {
		slave_warn(bond_dev, slave_dev, "no link monitoring support\n");
	}

	/* already in-use? */
	if (netdev_is_rx_handler_busy(slave_dev)) {
		NL_SET_ERR_MSG(extack, "Device is in use and cannot be enslaved");
		slave_err(bond_dev, slave_dev,
			  "Error: Device is in use and cannot be enslaved\n");
		return -EBUSY;
	}

	if (bond_dev == slave_dev) {
		NL_SET_ERR_MSG(extack, "Cannot enslave bond to itself.");
		netdev_err(bond_dev, "cannot enslave bond to itself.\n");
		return -EPERM;
	}

	/* vlan challenged mutual exclusion */
	/* no need to lock since we're protected by rtnl_lock */
	if (slave_dev->features & NETIF_F_VLAN_CHALLENGED) {
		slave_dbg(bond_dev, slave_dev, "is NETIF_F_VLAN_CHALLENGED\n");
		if (vlan_uses_dev(bond_dev)) {
			NL_SET_ERR_MSG(extack, "Can not enslave VLAN challenged device to VLAN enabled bond");
			slave_err(bond_dev, slave_dev, "Error: cannot enslave VLAN challenged slave on VLAN enabled bond\n");
			return -EPERM;
		} else {
			slave_warn(bond_dev, slave_dev, "enslaved VLAN challenged slave. Adding VLANs will be blocked as long as it is part of bond.\n");
		}
	} else {
		slave_dbg(bond_dev, slave_dev, "is !NETIF_F_VLAN_CHALLENGED\n");
	}

	if (slave_dev->features & NETIF_F_HW_ESP)
		slave_dbg(bond_dev, slave_dev, "is esp-hw-offload capable\n");

	/* Old ifenslave binaries are no longer supported.  These can
	 * be identified with moderate accuracy by the state of the slave:
	 * the current ifenslave will set the interface down prior to
	 * enslaving it; the old ifenslave will not.
	 */
	if (slave_dev->flags & IFF_UP) {
		NL_SET_ERR_MSG(extack, "Device can not be enslaved while up");
		slave_err(bond_dev, slave_dev, "slave is up - this may be due to an out of date ifenslave\n");
		return -EPERM;
	}

	/* set bonding device ether type by slave - bonding netdevices are
	 * created with ether_setup, so when the slave type is not ARPHRD_ETHER
	 * there is a need to override some of the type dependent attribs/funcs.
	 *
	 * bond ether type mutual exclusion - don't allow slaves of dissimilar
	 * ether type (eg ARPHRD_ETHER and ARPHRD_INFINIBAND) share the same bond
	 */
	if (!bond_has_slaves(bond)) {
		if (bond_dev->type != slave_dev->type) {
			slave_dbg(bond_dev, slave_dev, "change device type from %d to %d\n",
				  bond_dev->type, slave_dev->type);

			res = call_netdevice_notifiers(NETDEV_PRE_TYPE_CHANGE,
						       bond_dev);
			res = notifier_to_errno(res);
			if (res) {
				slave_err(bond_dev, slave_dev, "refused to change device type\n");
				return -EBUSY;
			}

			/* Flush unicast and multicast addresses */
			dev_uc_flush(bond_dev);
			dev_mc_flush(bond_dev);

			if (slave_dev->type != ARPHRD_ETHER)
				bond_setup_by_slave(bond_dev, slave_dev);
			else {
				ether_setup(bond_dev);
				bond_dev->priv_flags &= ~IFF_TX_SKB_SHARING;
			}

			call_netdevice_notifiers(NETDEV_POST_TYPE_CHANGE,
						 bond_dev);
		}
	} else if (bond_dev->type != slave_dev->type) {
		NL_SET_ERR_MSG(extack, "Device type is different from other slaves");
		slave_err(bond_dev, slave_dev, "ether type (%d) is different from other slaves (%d), can not enslave it\n",
			  slave_dev->type, bond_dev->type);
		return -EINVAL;
	}

	if (slave_dev->type == ARPHRD_INFINIBAND &&
	    BOND_MODE(bond) != BOND_MODE_ACTIVEBACKUP) {
		NL_SET_ERR_MSG(extack, "Only active-backup mode is supported for infiniband slaves");
		slave_warn(bond_dev, slave_dev, "Type (%d) supports only active-backup mode\n",
			   slave_dev->type);
		res = -EOPNOTSUPP;
		goto err_undo_flags;
	}

	if (!slave_ops->ndo_set_mac_address ||
	    slave_dev->type == ARPHRD_INFINIBAND) {
		slave_warn(bond_dev, slave_dev, "The slave device specified does not support setting the MAC address\n");
		if (BOND_MODE(bond) == BOND_MODE_ACTIVEBACKUP &&
		    bond->params.fail_over_mac != BOND_FOM_ACTIVE) {
			if (!bond_has_slaves(bond)) {
				bond->params.fail_over_mac = BOND_FOM_ACTIVE;
				slave_warn(bond_dev, slave_dev, "Setting fail_over_mac to active for active-backup mode\n");
			} else {
				NL_SET_ERR_MSG(extack, "Slave device does not support setting the MAC address, but fail_over_mac is not set to active");
				slave_err(bond_dev, slave_dev, "The slave device specified does not support setting the MAC address, but fail_over_mac is not set to active\n");
				res = -EOPNOTSUPP;
				goto err_undo_flags;
			}
		}
	}

	call_netdevice_notifiers(NETDEV_JOIN, slave_dev);

	/* If this is the first slave, then we need to set the master's hardware
	 * address to be the same as the slave's.
	 */
	if (!bond_has_slaves(bond) &&
	    bond->dev->addr_assign_type == NET_ADDR_RANDOM) {
		res = bond_set_dev_addr(bond->dev, slave_dev);
		if (res)
			goto err_undo_flags;
	}

	new_slave = bond_alloc_slave(bond, slave_dev);
	if (!new_slave) {
		res = -ENOMEM;
		goto err_undo_flags;
	}

	/* Set the new_slave's queue_id to be zero.  Queue ID mapping
	 * is set via sysfs or module option if desired.
	 */
	new_slave->queue_id = 0;

	/* Save slave's original mtu and then set it to match the bond */
	new_slave->original_mtu = slave_dev->mtu;
	res = dev_set_mtu(slave_dev, bond->dev->mtu);
	if (res) {
		slave_err(bond_dev, slave_dev, "Error %d calling dev_set_mtu\n", res);
		goto err_free;
	}

	/* Save slave's original ("permanent") mac address for modes
	 * that need it, and for restoring it upon release, and then
	 * set it to the master's address
	 */
	bond_hw_addr_copy(new_slave->perm_hwaddr, slave_dev->dev_addr,
			  slave_dev->addr_len);

	if (!bond->params.fail_over_mac ||
	    BOND_MODE(bond) != BOND_MODE_ACTIVEBACKUP) {
		/* Set slave to master's mac address.  The application already
		 * set the master's mac address to that of the first slave
		 */
		memcpy(ss.__data, bond_dev->dev_addr, bond_dev->addr_len);
		ss.ss_family = slave_dev->type;
		res = dev_set_mac_address(slave_dev, (struct sockaddr *)&ss,
					  extack);
		if (res) {
			slave_err(bond_dev, slave_dev, "Error %d calling set_mac_address\n", res);
			goto err_restore_mtu;
		}
	}

	/* set slave flag before open to prevent IPv6 addrconf */
	slave_dev->flags |= IFF_SLAVE;

	/* open the slave since the application closed it */
	res = dev_open(slave_dev, extack);
	if (res) {
		slave_err(bond_dev, slave_dev, "Opening slave failed\n");
		goto err_restore_mac;
	}

	slave_dev->priv_flags |= IFF_BONDING;
	/* initialize slave stats */
	dev_get_stats(new_slave->dev, &new_slave->slave_stats);

	if (bond_is_lb(bond)) {
		/* bond_alb_init_slave() must be called before all other stages since
		 * it might fail and we do not want to have to undo everything
		 */
		res = bond_alb_init_slave(bond, new_slave);
		if (res)
			goto err_close;
	}

	res = vlan_vids_add_by_dev(slave_dev, bond_dev);
	if (res) {
		slave_err(bond_dev, slave_dev, "Couldn't add bond vlan ids\n");
		goto err_close;
	}

	prev_slave = bond_last_slave(bond);

	new_slave->delay = 0;
	new_slave->link_failure_count = 0;

	if (bond_update_speed_duplex(new_slave) &&
	    bond_needs_speed_duplex(bond))
		new_slave->link = BOND_LINK_DOWN;

	new_slave->last_rx = jiffies -
		(msecs_to_jiffies(bond->params.arp_interval) + 1);
	for (i = 0; i < BOND_MAX_ARP_TARGETS; i++)
		new_slave->target_last_arp_rx[i] = new_slave->last_rx;

	if (bond->params.miimon && !bond->params.use_carrier) {
		link_reporting = bond_check_dev_link(bond, slave_dev, 1);

		if ((link_reporting == -1) && !bond->params.arp_interval) {
			/* miimon is set but a bonded network driver
			 * does not support ETHTOOL/MII and
			 * arp_interval is not set.  Note: if
			 * use_carrier is enabled, we will never go
			 * here (because netif_carrier is always
			 * supported); thus, we don't need to change
			 * the messages for netif_carrier.
			 */
			slave_warn(bond_dev, slave_dev, "MII and ETHTOOL support not available for slave, and arp_interval/arp_ip_target module parameters not specified, thus bonding will not detect link failures! see bonding.txt for details\n");
		} else if (link_reporting == -1) {
			/* unable get link status using mii/ethtool */
			slave_warn(bond_dev, slave_dev, "can't get link status from slave; the network driver associated with this interface does not support MII or ETHTOOL link status reporting, thus miimon has no effect on this interface\n");
		}
	}

	/* check for initial state */
	new_slave->link = BOND_LINK_NOCHANGE;
	if (bond->params.miimon) {
		if (bond_check_dev_link(bond, slave_dev, 0) == BMSR_LSTATUS) {
			if (bond->params.updelay) {
				bond_set_slave_link_state(new_slave,
							  BOND_LINK_BACK,
							  BOND_SLAVE_NOTIFY_NOW);
				new_slave->delay = bond->params.updelay;
			} else {
				bond_set_slave_link_state(new_slave,
							  BOND_LINK_UP,
							  BOND_SLAVE_NOTIFY_NOW);
			}
		} else {
			bond_set_slave_link_state(new_slave, BOND_LINK_DOWN,
						  BOND_SLAVE_NOTIFY_NOW);
		}
	} else if (bond->params.arp_interval) {
		bond_set_slave_link_state(new_slave,
					  (netif_carrier_ok(slave_dev) ?
					  BOND_LINK_UP : BOND_LINK_DOWN),
					  BOND_SLAVE_NOTIFY_NOW);
	} else {
		bond_set_slave_link_state(new_slave, BOND_LINK_UP,
					  BOND_SLAVE_NOTIFY_NOW);
	}

	if (new_slave->link != BOND_LINK_DOWN)
		new_slave->last_link_up = jiffies;
	slave_dbg(bond_dev, slave_dev, "Initial state of slave is BOND_LINK_%s\n",
		  new_slave->link == BOND_LINK_DOWN ? "DOWN" :
		  (new_slave->link == BOND_LINK_UP ? "UP" : "BACK"));

	if (bond_uses_primary(bond) && bond->params.primary[0]) {
		/* if there is a primary slave, remember it */
		if (strcmp(bond->params.primary, new_slave->dev->name) == 0) {
			rcu_assign_pointer(bond->primary_slave, new_slave);
			bond->force_primary = true;
		}
	}

	switch (BOND_MODE(bond)) {
	case BOND_MODE_ACTIVEBACKUP:
		bond_set_slave_inactive_flags(new_slave,
					      BOND_SLAVE_NOTIFY_NOW);
		break;
	case BOND_MODE_8023AD:
		/* in 802.3ad mode, the internal mechanism
		 * will activate the slaves in the selected
		 * aggregator
		 */
		bond_set_slave_inactive_flags(new_slave, BOND_SLAVE_NOTIFY_NOW);
		/* if this is the first slave */
		if (!prev_slave) {
			SLAVE_AD_INFO(new_slave)->id = 1;
			/* Initialize AD with the number of times that the AD timer is called in 1 second
			 * can be called only after the mac address of the bond is set
			 */
			bond_3ad_initialize(bond, 1000/AD_TIMER_INTERVAL);
		} else {
			SLAVE_AD_INFO(new_slave)->id =
				SLAVE_AD_INFO(prev_slave)->id + 1;
		}

		bond_3ad_bind_slave(new_slave);
		break;
	case BOND_MODE_TLB:
	case BOND_MODE_ALB:
		bond_set_active_slave(new_slave);
		bond_set_slave_inactive_flags(new_slave, BOND_SLAVE_NOTIFY_NOW);
		break;
	default:
		slave_dbg(bond_dev, slave_dev, "This slave is always active in trunk mode\n");

		/* always active in trunk mode */
		bond_set_active_slave(new_slave);

		/* In trunking mode there is little meaning to curr_active_slave
		 * anyway (it holds no special properties of the bond device),
		 * so we can change it without calling change_active_interface()
		 */
		if (!rcu_access_pointer(bond->curr_active_slave) &&
		    new_slave->link == BOND_LINK_UP)
			rcu_assign_pointer(bond->curr_active_slave, new_slave);

		break;
	} /* switch(bond_mode) */

#ifdef CONFIG_NET_POLL_CONTROLLER
	if (bond->dev->npinfo) {
		if (slave_enable_netpoll(new_slave)) {
			slave_info(bond_dev, slave_dev, "master_dev is using netpoll, but new slave device does not support netpoll\n");
			res = -EBUSY;
			goto err_detach;
		}
	}
#endif

	if (!(bond_dev->features & NETIF_F_LRO))
		dev_disable_lro(slave_dev);

	res = netdev_rx_handler_register(slave_dev, bond_handle_frame,
					 new_slave);
	if (res) {
		slave_dbg(bond_dev, slave_dev, "Error %d calling netdev_rx_handler_register\n", res);
		goto err_detach;
	}

	res = bond_master_upper_dev_link(bond, new_slave, extack);
	if (res) {
		slave_dbg(bond_dev, slave_dev, "Error %d calling bond_master_upper_dev_link\n", res);
		goto err_unregister;
	}

	bond_lower_state_changed(new_slave);

	res = bond_sysfs_slave_add(new_slave);
	if (res) {
		slave_dbg(bond_dev, slave_dev, "Error %d calling bond_sysfs_slave_add\n", res);
		goto err_upper_unlink;
	}

	/* If the mode uses primary, then the following is handled by
	 * bond_change_active_slave().
	 */
	if (!bond_uses_primary(bond)) {
		/* set promiscuity level to new slave */
		if (bond_dev->flags & IFF_PROMISC) {
			res = dev_set_promiscuity(slave_dev, 1);
			if (res)
				goto err_sysfs_del;
		}

		/* set allmulti level to new slave */
		if (bond_dev->flags & IFF_ALLMULTI) {
			res = dev_set_allmulti(slave_dev, 1);
			if (res) {
				if (bond_dev->flags & IFF_PROMISC)
					dev_set_promiscuity(slave_dev, -1);
				goto err_sysfs_del;
			}
		}

		netif_addr_lock_bh(bond_dev);
		dev_mc_sync_multiple(slave_dev, bond_dev);
		dev_uc_sync_multiple(slave_dev, bond_dev);
		netif_addr_unlock_bh(bond_dev);

		if (BOND_MODE(bond) == BOND_MODE_8023AD) {
			/* add lacpdu mc addr to mc list */
			u8 lacpdu_multicast[ETH_ALEN] = MULTICAST_LACPDU_ADDR;

			dev_mc_add(slave_dev, lacpdu_multicast);
		}
	}

	bond->slave_cnt++;
	bond_compute_features(bond);
	bond_set_carrier(bond);

	if (bond_uses_primary(bond)) {
		block_netpoll_tx();
		bond_select_active_slave(bond);
		unblock_netpoll_tx();
	}

	if (bond_mode_can_use_xmit_hash(bond))
		bond_update_slave_arr(bond, NULL);


	slave_info(bond_dev, slave_dev, "Enslaving as %s interface with %s link\n",
		   bond_is_active_slave(new_slave) ? "an active" : "a backup",
		   new_slave->link != BOND_LINK_DOWN ? "an up" : "a down");

	/* enslave is successful */
	bond_queue_slave_event(new_slave);
	return 0;

/* Undo stages on error */
err_sysfs_del:
	bond_sysfs_slave_del(new_slave);

err_upper_unlink:
	bond_upper_dev_unlink(bond, new_slave);

err_unregister:
	netdev_rx_handler_unregister(slave_dev);

err_detach:
	vlan_vids_del_by_dev(slave_dev, bond_dev);
	if (rcu_access_pointer(bond->primary_slave) == new_slave)
		RCU_INIT_POINTER(bond->primary_slave, NULL);
	if (rcu_access_pointer(bond->curr_active_slave) == new_slave) {
		block_netpoll_tx();
		bond_change_active_slave(bond, NULL);
		bond_select_active_slave(bond);
		unblock_netpoll_tx();
	}
	/* either primary_slave or curr_active_slave might've changed */
	synchronize_rcu();
	slave_disable_netpoll(new_slave);

err_close:
	if (!netif_is_bond_master(slave_dev))
		slave_dev->priv_flags &= ~IFF_BONDING;
	dev_close(slave_dev);

err_restore_mac:
	slave_dev->flags &= ~IFF_SLAVE;
	if (!bond->params.fail_over_mac ||
	    BOND_MODE(bond) != BOND_MODE_ACTIVEBACKUP) {
		/* XXX TODO - fom follow mode needs to change master's
		 * MAC if this slave's MAC is in use by the bond, or at
		 * least print a warning.
		 */
		bond_hw_addr_copy(ss.__data, new_slave->perm_hwaddr,
				  new_slave->dev->addr_len);
		ss.ss_family = slave_dev->type;
		dev_set_mac_address(slave_dev, (struct sockaddr *)&ss, NULL);
	}

err_restore_mtu:
	dev_set_mtu(slave_dev, new_slave->original_mtu);

err_free:
	kobject_put(&new_slave->kobj);

err_undo_flags:
	/* Enslave of first slave has failed and we need to fix master's mac */
	if (!bond_has_slaves(bond)) {
		if (ether_addr_equal_64bits(bond_dev->dev_addr,
					    slave_dev->dev_addr))
			eth_hw_addr_random(bond_dev);
		if (bond_dev->type != ARPHRD_ETHER) {
			dev_close(bond_dev);
			ether_setup(bond_dev);
			bond_dev->flags |= IFF_MASTER;
			bond_dev->priv_flags &= ~IFF_TX_SKB_SHARING;
		}
	}

	return res;
}