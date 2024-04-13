static enum netdev_lag_hash bond_lag_hash_type(struct bonding *bond,
					       enum netdev_lag_tx_type type)
{
	if (type != NETDEV_LAG_TX_TYPE_HASH)
		return NETDEV_LAG_HASH_NONE;

	switch (bond->params.xmit_policy) {
	case BOND_XMIT_POLICY_LAYER2:
		return NETDEV_LAG_HASH_L2;
	case BOND_XMIT_POLICY_LAYER34:
		return NETDEV_LAG_HASH_L34;
	case BOND_XMIT_POLICY_LAYER23:
		return NETDEV_LAG_HASH_L23;
	case BOND_XMIT_POLICY_ENCAP23:
		return NETDEV_LAG_HASH_E23;
	case BOND_XMIT_POLICY_ENCAP34:
		return NETDEV_LAG_HASH_E34;
	case BOND_XMIT_POLICY_VLAN_SRCMAC:
		return NETDEV_LAG_HASH_VLAN_SRCMAC;
	default:
		return NETDEV_LAG_HASH_UNKNOWN;
	}
}