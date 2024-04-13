static u8 i40e_pf_get_num_tc(struct i40e_pf *pf)
{
	struct i40e_hw *hw = &pf->hw;
	u8 i, enabled_tc = 1;
	u8 num_tc = 0;
	struct i40e_dcbx_config *dcbcfg = &hw->local_dcbx_config;

	if (pf->flags & I40E_FLAG_TC_MQPRIO)
		return pf->vsi[pf->lan_vsi]->mqprio_qopt.qopt.num_tc;

	/* If neither MQPRIO nor DCB is enabled, then always use single TC */
	if (!(pf->flags & I40E_FLAG_DCB_ENABLED))
		return 1;

	/* SFP mode will be enabled for all TCs on port */
	if (!(pf->flags & I40E_FLAG_MFP_ENABLED))
		return i40e_dcb_get_num_tc(dcbcfg);

	/* MFP mode return count of enabled TCs for this PF */
	if (pf->hw.func_caps.iscsi)
		enabled_tc =  i40e_get_iscsi_tc_map(pf);
	else
		return 1; /* Only TC0 */

	for (i = 0; i < I40E_MAX_TRAFFIC_CLASS; i++) {
		if (enabled_tc & BIT(i))
			num_tc++;
	}
	return num_tc;
}