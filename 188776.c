i40e_status i40e_set_partition_bw_setting(struct i40e_pf *pf)
{
	struct i40e_aqc_configure_partition_bw_data bw_data;
	i40e_status status;

	/* Set the valid bit for this PF */
	bw_data.pf_valid_bits = cpu_to_le16(BIT(pf->hw.pf_id));
	bw_data.max_bw[pf->hw.pf_id] = pf->max_bw & I40E_ALT_BW_VALUE_MASK;
	bw_data.min_bw[pf->hw.pf_id] = pf->min_bw & I40E_ALT_BW_VALUE_MASK;

	/* Set the new bandwidths */
	status = i40e_aq_configure_partition_bw(&pf->hw, &bw_data, NULL);

	return status;
}