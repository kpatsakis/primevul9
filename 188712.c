i40e_aqc_broadcast_filter(struct i40e_vsi *vsi, const char *vsi_name,
			  struct i40e_mac_filter *f)
{
	bool enable = f->state == I40E_FILTER_NEW;
	struct i40e_hw *hw = &vsi->back->hw;
	i40e_status aq_ret;

	if (f->vlan == I40E_VLAN_ANY) {
		aq_ret = i40e_aq_set_vsi_broadcast(hw,
						   vsi->seid,
						   enable,
						   NULL);
	} else {
		aq_ret = i40e_aq_set_vsi_bc_promisc_on_vlan(hw,
							    vsi->seid,
							    enable,
							    f->vlan,
							    NULL);
	}

	if (aq_ret) {
		set_bit(__I40E_VSI_OVERFLOW_PROMISC, vsi->state);
		dev_warn(&vsi->back->pdev->dev,
			 "Error %s, forcing overflow promiscuous on %s\n",
			 i40e_aq_str(hw, hw->aq.asq_last_status),
			 vsi_name);
	}

	return aq_ret;
}