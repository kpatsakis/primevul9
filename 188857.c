static int i40e_setup_tc_block_cb(enum tc_setup_type type, void *type_data,
				  void *cb_priv)
{
	struct i40e_netdev_priv *np = cb_priv;

	if (!tc_cls_can_offload_and_chain0(np->vsi->netdev, type_data))
		return -EOPNOTSUPP;

	switch (type) {
	case TC_SETUP_CLSFLOWER:
		return i40e_setup_tc_cls_flower(np, type_data);

	default:
		return -EOPNOTSUPP;
	}
}