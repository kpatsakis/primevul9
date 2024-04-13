static u8 i40e_pf_get_tc_map(struct i40e_pf *pf)
{
	if (pf->flags & I40E_FLAG_TC_MQPRIO)
		return i40e_mqprio_get_enabled_tc(pf);

	/* If neither MQPRIO nor DCB is enabled for this PF then just return
	 * default TC
	 */
	if (!(pf->flags & I40E_FLAG_DCB_ENABLED))
		return I40E_DEFAULT_TRAFFIC_CLASS;

	/* SFP mode we want PF to be enabled for all TCs */
	if (!(pf->flags & I40E_FLAG_MFP_ENABLED))
		return i40e_dcb_get_enabled_tc(&pf->hw.local_dcbx_config);

	/* MFP enabled and iSCSI PF type */
	if (pf->hw.func_caps.iscsi)
		return i40e_get_iscsi_tc_map(pf);
	else
		return I40E_DEFAULT_TRAFFIC_CLASS;
}