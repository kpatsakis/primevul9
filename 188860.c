static int __i40e_setup_tc(struct net_device *netdev, enum tc_setup_type type,
			   void *type_data)
{
	struct i40e_netdev_priv *np = netdev_priv(netdev);

	switch (type) {
	case TC_SETUP_QDISC_MQPRIO:
		return i40e_setup_tc(netdev, type_data);
	case TC_SETUP_BLOCK:
		return flow_block_cb_setup_simple(type_data,
						  &i40e_block_cb_list,
						  i40e_setup_tc_block_cb,
						  np, np, true);
	default:
		return -EOPNOTSUPP;
	}
}