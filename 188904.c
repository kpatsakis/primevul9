struct rtnl_link_stats64 *i40e_get_vsi_stats_struct(struct i40e_vsi *vsi)
{
	return &vsi->net_stats;
}