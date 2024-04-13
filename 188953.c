void i40e_aqc_del_filters(struct i40e_vsi *vsi, const char *vsi_name,
			  struct i40e_aqc_remove_macvlan_element_data *list,
			  int num_del, int *retval)
{
	struct i40e_hw *hw = &vsi->back->hw;
	i40e_status aq_ret;
	int aq_err;

	aq_ret = i40e_aq_remove_macvlan(hw, vsi->seid, list, num_del, NULL);
	aq_err = hw->aq.asq_last_status;

	/* Explicitly ignore and do not report when firmware returns ENOENT */
	if (aq_ret && !(aq_err == I40E_AQ_RC_ENOENT)) {
		*retval = -EIO;
		dev_info(&vsi->back->pdev->dev,
			 "ignoring delete macvlan error on %s, err %s, aq_err %s\n",
			 vsi_name, i40e_stat_str(hw, aq_ret),
			 i40e_aq_str(hw, aq_err));
	}
}