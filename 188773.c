static int i40e_setup_tc_cls_flower(struct i40e_netdev_priv *np,
				    struct flow_cls_offload *cls_flower)
{
	struct i40e_vsi *vsi = np->vsi;

	switch (cls_flower->command) {
	case FLOW_CLS_REPLACE:
		return i40e_configure_clsflower(vsi, cls_flower);
	case FLOW_CLS_DESTROY:
		return i40e_delete_clsflower(vsi, cls_flower);
	case FLOW_CLS_STATS:
		return -EOPNOTSUPP;
	default:
		return -EOPNOTSUPP;
	}
}