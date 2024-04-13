static int i40e_configure_clsflower(struct i40e_vsi *vsi,
				    struct flow_cls_offload *cls_flower)
{
	int tc = tc_classid_to_hwtc(vsi->netdev, cls_flower->classid);
	struct i40e_cloud_filter *filter = NULL;
	struct i40e_pf *pf = vsi->back;
	int err = 0;

	if (tc < 0) {
		dev_err(&vsi->back->pdev->dev, "Invalid traffic class\n");
		return -EOPNOTSUPP;
	}

	if (test_bit(__I40E_RESET_RECOVERY_PENDING, pf->state) ||
	    test_bit(__I40E_RESET_INTR_RECEIVED, pf->state))
		return -EBUSY;

	if (pf->fdir_pf_active_filters ||
	    (!hlist_empty(&pf->fdir_filter_list))) {
		dev_err(&vsi->back->pdev->dev,
			"Flow Director Sideband filters exists, turn ntuple off to configure cloud filters\n");
		return -EINVAL;
	}

	if (vsi->back->flags & I40E_FLAG_FD_SB_ENABLED) {
		dev_err(&vsi->back->pdev->dev,
			"Disable Flow Director Sideband, configuring Cloud filters via tc-flower\n");
		vsi->back->flags &= ~I40E_FLAG_FD_SB_ENABLED;
		vsi->back->flags |= I40E_FLAG_FD_SB_TO_CLOUD_FILTER;
	}

	filter = kzalloc(sizeof(*filter), GFP_KERNEL);
	if (!filter)
		return -ENOMEM;

	filter->cookie = cls_flower->cookie;

	err = i40e_parse_cls_flower(vsi, cls_flower, filter);
	if (err < 0)
		goto err;

	err = i40e_handle_tclass(vsi, tc, filter);
	if (err < 0)
		goto err;

	/* Add cloud filter */
	if (filter->dst_port)
		err = i40e_add_del_cloud_filter_big_buf(vsi, filter, true);
	else
		err = i40e_add_del_cloud_filter(vsi, filter, true);

	if (err) {
		dev_err(&pf->pdev->dev,
			"Failed to add cloud filter, err %s\n",
			i40e_stat_str(&pf->hw, err));
		goto err;
	}

	/* add filter to the ordered list */
	INIT_HLIST_NODE(&filter->cloud_node);

	hlist_add_head(&filter->cloud_node, &pf->cloud_filter_list);

	pf->num_cloud_filters++;

	return err;
err:
	kfree(filter);
	return err;
}