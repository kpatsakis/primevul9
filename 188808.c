int i40e_add_del_cloud_filter(struct i40e_vsi *vsi,
			      struct i40e_cloud_filter *filter, bool add)
{
	struct i40e_aqc_cloud_filters_element_data cld_filter;
	struct i40e_pf *pf = vsi->back;
	int ret;
	static const u16 flag_table[128] = {
		[I40E_CLOUD_FILTER_FLAGS_OMAC]  =
			I40E_AQC_ADD_CLOUD_FILTER_OMAC,
		[I40E_CLOUD_FILTER_FLAGS_IMAC]  =
			I40E_AQC_ADD_CLOUD_FILTER_IMAC,
		[I40E_CLOUD_FILTER_FLAGS_IMAC_IVLAN]  =
			I40E_AQC_ADD_CLOUD_FILTER_IMAC_IVLAN,
		[I40E_CLOUD_FILTER_FLAGS_IMAC_TEN_ID] =
			I40E_AQC_ADD_CLOUD_FILTER_IMAC_TEN_ID,
		[I40E_CLOUD_FILTER_FLAGS_OMAC_TEN_ID_IMAC] =
			I40E_AQC_ADD_CLOUD_FILTER_OMAC_TEN_ID_IMAC,
		[I40E_CLOUD_FILTER_FLAGS_IMAC_IVLAN_TEN_ID] =
			I40E_AQC_ADD_CLOUD_FILTER_IMAC_IVLAN_TEN_ID,
		[I40E_CLOUD_FILTER_FLAGS_IIP] =
			I40E_AQC_ADD_CLOUD_FILTER_IIP,
	};

	if (filter->flags >= ARRAY_SIZE(flag_table))
		return I40E_ERR_CONFIG;

	/* copy element needed to add cloud filter from filter */
	i40e_set_cld_element(filter, &cld_filter);

	if (filter->tunnel_type != I40E_CLOUD_TNL_TYPE_NONE)
		cld_filter.flags = cpu_to_le16(filter->tunnel_type <<
					     I40E_AQC_ADD_CLOUD_TNL_TYPE_SHIFT);

	if (filter->n_proto == ETH_P_IPV6)
		cld_filter.flags |= cpu_to_le16(flag_table[filter->flags] |
						I40E_AQC_ADD_CLOUD_FLAGS_IPV6);
	else
		cld_filter.flags |= cpu_to_le16(flag_table[filter->flags] |
						I40E_AQC_ADD_CLOUD_FLAGS_IPV4);

	if (add)
		ret = i40e_aq_add_cloud_filters(&pf->hw, filter->seid,
						&cld_filter, 1);
	else
		ret = i40e_aq_rem_cloud_filters(&pf->hw, filter->seid,
						&cld_filter, 1);
	if (ret)
		dev_dbg(&pf->pdev->dev,
			"Failed to %s cloud filter using l4 port %u, err %d aq_err %d\n",
			add ? "add" : "delete", filter->dst_port, ret,
			pf->hw.aq.asq_last_status);
	else
		dev_info(&pf->pdev->dev,
			 "%s cloud filter for VSI: %d\n",
			 add ? "Added" : "Deleted", filter->seid);
	return ret;
}