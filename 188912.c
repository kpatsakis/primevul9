static int i40e_vsi_enable_irq(struct i40e_vsi *vsi)
{
	struct i40e_pf *pf = vsi->back;
	int i;

	if (pf->flags & I40E_FLAG_MSIX_ENABLED) {
		for (i = 0; i < vsi->num_q_vectors; i++)
			i40e_irq_dynamic_enable(vsi, i);
	} else {
		i40e_irq_dynamic_enable_icr0(pf);
	}

	i40e_flush(&pf->hw);
	return 0;
}