static void i40e_fdir_filter_restore(struct i40e_vsi *vsi)
{
	struct i40e_fdir_filter *filter;
	struct i40e_pf *pf = vsi->back;
	struct hlist_node *node;

	if (!(pf->flags & I40E_FLAG_FD_SB_ENABLED))
		return;

	/* Reset FDir counters as we're replaying all existing filters */
	pf->fd_tcp4_filter_cnt = 0;
	pf->fd_udp4_filter_cnt = 0;
	pf->fd_sctp4_filter_cnt = 0;
	pf->fd_ip4_filter_cnt = 0;

	hlist_for_each_entry_safe(filter, node,
				  &pf->fdir_filter_list, fdir_node) {
		i40e_add_del_fdir(vsi, filter, true);
	}
}