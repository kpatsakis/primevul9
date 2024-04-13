static int bond_master_upper_dev_link(struct bonding *bond, struct slave *slave,
				      struct netlink_ext_ack *extack)
{
	struct netdev_lag_upper_info lag_upper_info;
	enum netdev_lag_tx_type type;

	type = bond_lag_tx_type(bond);
	lag_upper_info.tx_type = type;
	lag_upper_info.hash_type = bond_lag_hash_type(bond, type);

	return netdev_master_upper_dev_link(slave->dev, bond->dev, slave,
					    &lag_upper_info, extack);
}