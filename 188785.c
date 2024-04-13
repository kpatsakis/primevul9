static int i40e_handle_tclass(struct i40e_vsi *vsi, u32 tc,
			      struct i40e_cloud_filter *filter)
{
	struct i40e_channel *ch, *ch_tmp;

	/* direct to a traffic class on the same device */
	if (tc == 0) {
		filter->seid = vsi->seid;
		return 0;
	} else if (vsi->tc_config.enabled_tc & BIT(tc)) {
		if (!filter->dst_port) {
			dev_err(&vsi->back->pdev->dev,
				"Specify destination port to direct to traffic class that is not default\n");
			return -EINVAL;
		}
		if (list_empty(&vsi->ch_list))
			return -EINVAL;
		list_for_each_entry_safe(ch, ch_tmp, &vsi->ch_list,
					 list) {
			if (ch->seid == vsi->tc_seid_map[tc])
				filter->seid = ch->seid;
		}
		return 0;
	}
	dev_err(&vsi->back->pdev->dev, "TC is not enabled\n");
	return -EINVAL;
}