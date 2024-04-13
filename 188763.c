int i40e_vsi_add_pvid(struct i40e_vsi *vsi, u16 vid)
{
	struct i40e_vsi_context ctxt;
	i40e_status ret;

	vsi->info.valid_sections = cpu_to_le16(I40E_AQ_VSI_PROP_VLAN_VALID);
	vsi->info.pvid = cpu_to_le16(vid);
	vsi->info.port_vlan_flags = I40E_AQ_VSI_PVLAN_MODE_TAGGED |
				    I40E_AQ_VSI_PVLAN_INSERT_PVID |
				    I40E_AQ_VSI_PVLAN_EMOD_STR;

	ctxt.seid = vsi->seid;
	ctxt.info = vsi->info;
	ret = i40e_aq_update_vsi_params(&vsi->back->hw, &ctxt, NULL);
	if (ret) {
		dev_info(&vsi->back->pdev->dev,
			 "add pvid failed, err %s aq_err %s\n",
			 i40e_stat_str(&vsi->back->hw, ret),
			 i40e_aq_str(&vsi->back->hw,
				     vsi->back->hw.aq.asq_last_status));
		return -ENOENT;
	}

	return 0;
}