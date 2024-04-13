static int i40e_set_promiscuous(struct i40e_pf *pf, bool promisc)
{
	struct i40e_vsi *vsi = pf->vsi[pf->lan_vsi];
	struct i40e_hw *hw = &pf->hw;
	i40e_status aq_ret;

	if (vsi->type == I40E_VSI_MAIN &&
	    pf->lan_veb != I40E_NO_VEB &&
	    !(pf->flags & I40E_FLAG_MFP_ENABLED)) {
		/* set defport ON for Main VSI instead of true promisc
		 * this way we will get all unicast/multicast and VLAN
		 * promisc behavior but will not get VF or VMDq traffic
		 * replicated on the Main VSI.
		 */
		if (promisc)
			aq_ret = i40e_aq_set_default_vsi(hw,
							 vsi->seid,
							 NULL);
		else
			aq_ret = i40e_aq_clear_default_vsi(hw,
							   vsi->seid,
							   NULL);
		if (aq_ret) {
			dev_info(&pf->pdev->dev,
				 "Set default VSI failed, err %s, aq_err %s\n",
				 i40e_stat_str(hw, aq_ret),
				 i40e_aq_str(hw, hw->aq.asq_last_status));
		}
	} else {
		aq_ret = i40e_aq_set_vsi_unicast_promiscuous(
						  hw,
						  vsi->seid,
						  promisc, NULL,
						  true);
		if (aq_ret) {
			dev_info(&pf->pdev->dev,
				 "set unicast promisc failed, err %s, aq_err %s\n",
				 i40e_stat_str(hw, aq_ret),
				 i40e_aq_str(hw, hw->aq.asq_last_status));
		}
		aq_ret = i40e_aq_set_vsi_multicast_promiscuous(
						  hw,
						  vsi->seid,
						  promisc, NULL);
		if (aq_ret) {
			dev_info(&pf->pdev->dev,
				 "set multicast promisc failed, err %s, aq_err %s\n",
				 i40e_stat_str(hw, aq_ret),
				 i40e_aq_str(hw, hw->aq.asq_last_status));
		}
	}

	if (!aq_ret)
		pf->cur_promisc = promisc;

	return aq_ret;
}