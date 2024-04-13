void i40e_update_stats(struct i40e_vsi *vsi)
{
	struct i40e_pf *pf = vsi->back;

	if (vsi == pf->vsi[pf->lan_vsi])
		i40e_update_pf_stats(pf);

	i40e_update_vsi_stats(vsi);
}