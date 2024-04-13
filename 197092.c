void bond_lower_state_changed(struct slave *slave)
{
	struct netdev_lag_lower_state_info info;

	info.link_up = slave->link == BOND_LINK_UP ||
		       slave->link == BOND_LINK_FAIL;
	info.tx_enabled = bond_is_active_slave(slave);
	netdev_lower_state_changed(slave->dev, &info);
}