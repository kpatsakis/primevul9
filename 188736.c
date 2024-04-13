static void i40e_vsi_disable_irq(struct i40e_vsi *vsi)
{
	struct i40e_pf *pf = vsi->back;
	struct i40e_hw *hw = &pf->hw;
	int base = vsi->base_vector;
	int i;

	/* disable interrupt causation from each queue */
	for (i = 0; i < vsi->num_queue_pairs; i++) {
		u32 val;

		val = rd32(hw, I40E_QINT_TQCTL(vsi->tx_rings[i]->reg_idx));
		val &= ~I40E_QINT_TQCTL_CAUSE_ENA_MASK;
		wr32(hw, I40E_QINT_TQCTL(vsi->tx_rings[i]->reg_idx), val);

		val = rd32(hw, I40E_QINT_RQCTL(vsi->rx_rings[i]->reg_idx));
		val &= ~I40E_QINT_RQCTL_CAUSE_ENA_MASK;
		wr32(hw, I40E_QINT_RQCTL(vsi->rx_rings[i]->reg_idx), val);

		if (!i40e_enabled_xdp_vsi(vsi))
			continue;
		wr32(hw, I40E_QINT_TQCTL(vsi->xdp_rings[i]->reg_idx), 0);
	}

	/* disable each interrupt */
	if (pf->flags & I40E_FLAG_MSIX_ENABLED) {
		for (i = vsi->base_vector;
		     i < (vsi->num_q_vectors + vsi->base_vector); i++)
			wr32(hw, I40E_PFINT_DYN_CTLN(i - 1), 0);

		i40e_flush(hw);
		for (i = 0; i < vsi->num_q_vectors; i++)
			synchronize_irq(pf->msix_entries[i + base].vector);
	} else {
		/* Legacy and MSI mode - this stops all interrupt handling */
		wr32(hw, I40E_PFINT_ICR0_ENA, 0);
		wr32(hw, I40E_PFINT_DYN_CTL0, 0);
		i40e_flush(hw);
		synchronize_irq(pf->pdev->irq);
	}
}