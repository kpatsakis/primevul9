static int bond_slave_netdev_event(unsigned long event,
				   struct net_device *slave_dev)
{
	struct slave *slave = bond_slave_get_rtnl(slave_dev), *primary;
	struct bonding *bond;
	struct net_device *bond_dev;

	/* A netdev event can be generated while enslaving a device
	 * before netdev_rx_handler_register is called in which case
	 * slave will be NULL
	 */
	if (!slave) {
		netdev_dbg(slave_dev, "%s called on NULL slave\n", __func__);
		return NOTIFY_DONE;
	}

	bond_dev = slave->bond->dev;
	bond = slave->bond;
	primary = rtnl_dereference(bond->primary_slave);

	slave_dbg(bond_dev, slave_dev, "%s called\n", __func__);

	switch (event) {
	case NETDEV_UNREGISTER:
		if (bond_dev->type != ARPHRD_ETHER)
			bond_release_and_destroy(bond_dev, slave_dev);
		else
			__bond_release_one(bond_dev, slave_dev, false, true);
		break;
	case NETDEV_UP:
	case NETDEV_CHANGE:
		/* For 802.3ad mode only:
		 * Getting invalid Speed/Duplex values here will put slave
		 * in weird state. Mark it as link-fail if the link was
		 * previously up or link-down if it hasn't yet come up, and
		 * let link-monitoring (miimon) set it right when correct
		 * speeds/duplex are available.
		 */
		if (bond_update_speed_duplex(slave) &&
		    BOND_MODE(bond) == BOND_MODE_8023AD) {
			if (slave->last_link_up)
				slave->link = BOND_LINK_FAIL;
			else
				slave->link = BOND_LINK_DOWN;
		}

		if (BOND_MODE(bond) == BOND_MODE_8023AD)
			bond_3ad_adapter_speed_duplex_changed(slave);
		fallthrough;
	case NETDEV_DOWN:
		/* Refresh slave-array if applicable!
		 * If the setup does not use miimon or arpmon (mode-specific!),
		 * then these events will not cause the slave-array to be
		 * refreshed. This will cause xmit to use a slave that is not
		 * usable. Avoid such situation by refeshing the array at these
		 * events. If these (miimon/arpmon) parameters are configured
		 * then array gets refreshed twice and that should be fine!
		 */
		if (bond_mode_can_use_xmit_hash(bond))
			bond_update_slave_arr(bond, NULL);
		break;
	case NETDEV_CHANGEMTU:
		/* TODO: Should slaves be allowed to
		 * independently alter their MTU?  For
		 * an active-backup bond, slaves need
		 * not be the same type of device, so
		 * MTUs may vary.  For other modes,
		 * slaves arguably should have the
		 * same MTUs. To do this, we'd need to
		 * take over the slave's change_mtu
		 * function for the duration of their
		 * servitude.
		 */
		break;
	case NETDEV_CHANGENAME:
		/* we don't care if we don't have primary set */
		if (!bond_uses_primary(bond) ||
		    !bond->params.primary[0])
			break;

		if (slave == primary) {
			/* slave's name changed - he's no longer primary */
			RCU_INIT_POINTER(bond->primary_slave, NULL);
		} else if (!strcmp(slave_dev->name, bond->params.primary)) {
			/* we have a new primary slave */
			rcu_assign_pointer(bond->primary_slave, slave);
		} else { /* we didn't change primary - exit */
			break;
		}

		netdev_info(bond->dev, "Primary slave changed to %s, reselecting active slave\n",
			    primary ? slave_dev->name : "none");

		block_netpoll_tx();
		bond_select_active_slave(bond);
		unblock_netpoll_tx();
		break;
	case NETDEV_FEAT_CHANGE:
		bond_compute_features(bond);
		break;
	case NETDEV_RESEND_IGMP:
		/* Propagate to master device */
		call_netdevice_notifiers(event, slave->bond->dev);
		break;
	default:
		break;
	}

	return NOTIFY_DONE;
}