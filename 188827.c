static void i40e_queue_pair_enable_irq(struct i40e_vsi *vsi, int queue_pair)
{
	struct i40e_ring *rxr = vsi->rx_rings[queue_pair];
	struct i40e_pf *pf = vsi->back;
	struct i40e_hw *hw = &pf->hw;

	/* All rings in a qp belong to the same qvector. */
	if (pf->flags & I40E_FLAG_MSIX_ENABLED)
		i40e_irq_dynamic_enable(vsi, rxr->q_vector->v_idx);
	else
		i40e_irq_dynamic_enable_icr0(pf);

	i40e_flush(hw);
}