static int bond_ethtool_get_link_ksettings(struct net_device *bond_dev,
					   struct ethtool_link_ksettings *cmd)
{
	struct bonding *bond = netdev_priv(bond_dev);
	struct list_head *iter;
	struct slave *slave;
	u32 speed = 0;

	cmd->base.duplex = DUPLEX_UNKNOWN;
	cmd->base.port = PORT_OTHER;

	/* Since bond_slave_can_tx returns false for all inactive or down slaves, we
	 * do not need to check mode.  Though link speed might not represent
	 * the true receive or transmit bandwidth (not all modes are symmetric)
	 * this is an accurate maximum.
	 */
	bond_for_each_slave(bond, slave, iter) {
		if (bond_slave_can_tx(slave)) {
			if (slave->speed != SPEED_UNKNOWN) {
				if (BOND_MODE(bond) == BOND_MODE_BROADCAST)
					speed = bond_mode_bcast_speed(slave,
								      speed);
				else
					speed += slave->speed;
			}
			if (cmd->base.duplex == DUPLEX_UNKNOWN &&
			    slave->duplex != DUPLEX_UNKNOWN)
				cmd->base.duplex = slave->duplex;
		}
	}
	cmd->base.speed = speed ? : SPEED_UNKNOWN;

	return 0;
}