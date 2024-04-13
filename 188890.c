static void i40e_fdir_teardown(struct i40e_pf *pf)
{
	struct i40e_vsi *vsi;

	i40e_fdir_filter_exit(pf);
	vsi = i40e_find_vsi_by_type(pf, I40E_VSI_FDIR);
	if (vsi)
		i40e_vsi_release(vsi);
}