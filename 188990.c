static void i40e_veb_link_event(struct i40e_veb *veb, bool link_up)
{
	struct i40e_pf *pf;
	int i;

	if (!veb || !veb->pf)
		return;
	pf = veb->pf;

	/* depth first... */
	for (i = 0; i < I40E_MAX_VEB; i++)
		if (pf->veb[i] && (pf->veb[i]->uplink_seid == veb->seid))
			i40e_veb_link_event(pf->veb[i], link_up);

	/* ... now the local VSIs */
	for (i = 0; i < pf->num_alloc_vsi; i++)
		if (pf->vsi[i] && (pf->vsi[i]->uplink_seid == veb->seid))
			i40e_vsi_link_event(pf->vsi[i], link_up);
}