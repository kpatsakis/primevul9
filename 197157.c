static enum netdev_lag_tx_type bond_lag_tx_type(struct bonding *bond)
{
	switch (BOND_MODE(bond)) {
	case BOND_MODE_ROUNDROBIN:
		return NETDEV_LAG_TX_TYPE_ROUNDROBIN;
	case BOND_MODE_ACTIVEBACKUP:
		return NETDEV_LAG_TX_TYPE_ACTIVEBACKUP;
	case BOND_MODE_BROADCAST:
		return NETDEV_LAG_TX_TYPE_BROADCAST;
	case BOND_MODE_XOR:
	case BOND_MODE_8023AD:
		return NETDEV_LAG_TX_TYPE_HASH;
	default:
		return NETDEV_LAG_TX_TYPE_UNKNOWN;
	}
}