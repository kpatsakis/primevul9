static void i40e_cloud_filter_exit(struct i40e_pf *pf)
{
	struct i40e_cloud_filter *cfilter;
	struct hlist_node *node;

	hlist_for_each_entry_safe(cfilter, node,
				  &pf->cloud_filter_list, cloud_node) {
		hlist_del(&cfilter->cloud_node);
		kfree(cfilter);
	}
	pf->num_cloud_filters = 0;

	if ((pf->flags & I40E_FLAG_FD_SB_TO_CLOUD_FILTER) &&
	    !(pf->flags & I40E_FLAG_FD_SB_INACTIVE)) {
		pf->flags |= I40E_FLAG_FD_SB_ENABLED;
		pf->flags &= ~I40E_FLAG_FD_SB_TO_CLOUD_FILTER;
		pf->flags &= ~I40E_FLAG_FD_SB_INACTIVE;
	}
}