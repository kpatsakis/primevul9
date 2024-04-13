static void i40e_pf_unquiesce_all_vsi(struct i40e_pf *pf)
{
	int v;

	for (v = 0; v < pf->num_alloc_vsi; v++) {
		if (pf->vsi[v])
			i40e_unquiesce_vsi(pf->vsi[v]);
	}
}