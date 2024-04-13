void i40e_vlan_stripping_enable(struct i40e_vsi *vsi)
{
	struct i40e_vsi_context ctxt;
	i40e_status ret;

	/* Don't modify stripping options if a port VLAN is active */
	if (vsi->info.pvid)
		return;

	if ((vsi->info.valid_sections &
	     cpu_to_le16(I40E_AQ_VSI_PROP_VLAN_VALID)) &&
	    ((vsi->info.port_vlan_flags & I40E_AQ_VSI_PVLAN_MODE_MASK) == 0))
		return;  /* already enabled */

	vsi->info.valid_sections = cpu_to_le16(I40E_AQ_VSI_PROP_VLAN_VALID);
	vsi->info.port_vlan_flags = I40E_AQ_VSI_PVLAN_MODE_ALL |
				    I40E_AQ_VSI_PVLAN_EMOD_STR_BOTH;

	ctxt.seid = vsi->seid;
	ctxt.info = vsi->info;
	ret = i40e_aq_update_vsi_params(&vsi->back->hw, &ctxt, NULL);
	if (ret) {
		dev_info(&vsi->back->pdev->dev,
			 "update vlan stripping failed, err %s aq_err %s\n",
			 i40e_stat_str(&vsi->back->hw, ret),
			 i40e_aq_str(&vsi->back->hw,
				     vsi->back->hw.aq.asq_last_status));
	}
}