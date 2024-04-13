i40e_status i40e_get_partition_bw_setting(struct i40e_pf *pf)
{
	i40e_status status;
	bool min_valid, max_valid;
	u32 max_bw, min_bw;

	status = i40e_read_bw_from_alt_ram(&pf->hw, &max_bw, &min_bw,
					   &min_valid, &max_valid);

	if (!status) {
		if (min_valid)
			pf->min_bw = min_bw;
		if (max_valid)
			pf->max_bw = max_bw;
	}

	return status;
}