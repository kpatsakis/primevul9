void i40e_aqc_add_filters(struct i40e_vsi *vsi, const char *vsi_name,
			  struct i40e_aqc_add_macvlan_element_data *list,
			  struct i40e_new_mac_filter *add_head,
			  int num_add)
{
	struct i40e_hw *hw = &vsi->back->hw;
	int aq_err, fcnt;

	i40e_aq_add_macvlan(hw, vsi->seid, list, num_add, NULL);
	aq_err = hw->aq.asq_last_status;
	fcnt = i40e_update_filter_state(num_add, list, add_head);

	if (fcnt != num_add) {
		if (vsi->type == I40E_VSI_MAIN) {
			set_bit(__I40E_VSI_OVERFLOW_PROMISC, vsi->state);
			dev_warn(&vsi->back->pdev->dev,
				 "Error %s adding RX filters on %s, promiscuous mode forced on\n",
				 i40e_aq_str(hw, aq_err), vsi_name);
		} else if (vsi->type == I40E_VSI_SRIOV ||
			   vsi->type == I40E_VSI_VMDQ1 ||
			   vsi->type == I40E_VSI_VMDQ2) {
			dev_warn(&vsi->back->pdev->dev,
				 "Error %s adding RX filters on %s, please set promiscuous on manually for %s\n",
				 i40e_aq_str(hw, aq_err), vsi_name, vsi_name);
		} else {
			dev_warn(&vsi->back->pdev->dev,
				 "Error %s adding RX filters on %s, incorrect VSI type: %i.\n",
				 i40e_aq_str(hw, aq_err), vsi_name, vsi->type);
		}
	}
}