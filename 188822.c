static void i40e_queue_pair_disable_irq(struct i40e_vsi *vsi, int queue_pair)
{
	struct i40e_ring *rxr = vsi->rx_rings[queue_pair];
	struct i40e_pf *pf = vsi->back;
	struct i40e_hw *hw = &pf->hw;

	/* For simplicity, instead of removing the qp interrupt causes
	 * from the interrupt linked list, we simply disable the interrupt, and
	 * leave the list intact.
	 *
	 * All rings in a qp belong to the same qvector.
	 */
	if (pf->flags & I40E_FLAG_MSIX_ENABLED) {
		u32 intpf = vsi->base_vector + rxr->q_vector->v_idx;

		wr32(hw, I40E_PFINT_DYN_CTLN(intpf - 1), 0);
		i40e_flush(hw);
		synchronize_irq(pf->msix_entries[intpf].vector);
	} else {
		/* Legacy and MSI mode - this stops all interrupt handling */
		wr32(hw, I40E_PFINT_ICR0_ENA, 0);
		wr32(hw, I40E_PFINT_DYN_CTL0, 0);
		i40e_flush(hw);
		synchronize_irq(pf->pdev->irq);
	}
}