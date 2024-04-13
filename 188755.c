static int i40e_delete_clsflower(struct i40e_vsi *vsi,
				 struct flow_cls_offload *cls_flower)
{
	struct i40e_cloud_filter *filter = NULL;
	struct i40e_pf *pf = vsi->back;
	int err = 0;

	filter = i40e_find_cloud_filter(vsi, &cls_flower->cookie);

	if (!filter)
		return -EINVAL;

	hash_del(&filter->cloud_node);

	if (filter->dst_port)
		err = i40e_add_del_cloud_filter_big_buf(vsi, filter, false);
	else
		err = i40e_add_del_cloud_filter(vsi, filter, false);

	kfree(filter);
	if (err) {
		dev_err(&pf->pdev->dev,
			"Failed to delete cloud filter, err %s\n",
			i40e_stat_str(&pf->hw, err));
		return i40e_aq_rc_to_posix(err, pf->hw.aq.asq_last_status);
	}

	pf->num_cloud_filters--;
	if (!pf->num_cloud_filters)
		if ((pf->flags & I40E_FLAG_FD_SB_TO_CLOUD_FILTER) &&
		    !(pf->flags & I40E_FLAG_FD_SB_INACTIVE)) {
			pf->flags |= I40E_FLAG_FD_SB_ENABLED;
			pf->flags &= ~I40E_FLAG_FD_SB_TO_CLOUD_FILTER;
			pf->flags &= ~I40E_FLAG_FD_SB_INACTIVE;
		}
	return 0;
}