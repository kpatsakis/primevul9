static void i40e_configure_msi_and_legacy(struct i40e_vsi *vsi)
{
	u32 nextqp = i40e_enabled_xdp_vsi(vsi) ? vsi->alloc_queue_pairs : 0;
	struct i40e_q_vector *q_vector = vsi->q_vectors[0];
	struct i40e_pf *pf = vsi->back;
	struct i40e_hw *hw = &pf->hw;
	u32 val;

	/* set the ITR configuration */
	q_vector->rx.next_update = jiffies + 1;
	q_vector->rx.target_itr = ITR_TO_REG(vsi->rx_rings[0]->itr_setting);
	wr32(hw, I40E_PFINT_ITR0(I40E_RX_ITR), q_vector->rx.target_itr >> 1);
	q_vector->rx.current_itr = q_vector->rx.target_itr;
	q_vector->tx.next_update = jiffies + 1;
	q_vector->tx.target_itr = ITR_TO_REG(vsi->tx_rings[0]->itr_setting);
	wr32(hw, I40E_PFINT_ITR0(I40E_TX_ITR), q_vector->tx.target_itr >> 1);
	q_vector->tx.current_itr = q_vector->tx.target_itr;

	i40e_enable_misc_int_causes(pf);

	/* FIRSTQ_INDX = 0, FIRSTQ_TYPE = 0 (rx) */
	wr32(hw, I40E_PFINT_LNKLST0, 0);

	/* Associate the queue pair to the vector and enable the queue int */
	val = I40E_QINT_RQCTL_CAUSE_ENA_MASK		       |
	      (I40E_RX_ITR << I40E_QINT_RQCTL_ITR_INDX_SHIFT)  |
	      (nextqp	   << I40E_QINT_RQCTL_NEXTQ_INDX_SHIFT)|
	      (I40E_QUEUE_TYPE_TX << I40E_QINT_TQCTL_NEXTQ_TYPE_SHIFT);

	wr32(hw, I40E_QINT_RQCTL(0), val);

	if (i40e_enabled_xdp_vsi(vsi)) {
		val = I40E_QINT_TQCTL_CAUSE_ENA_MASK		     |
		      (I40E_TX_ITR << I40E_QINT_TQCTL_ITR_INDX_SHIFT)|
		      (I40E_QUEUE_TYPE_TX
		       << I40E_QINT_TQCTL_NEXTQ_TYPE_SHIFT);

		wr32(hw, I40E_QINT_TQCTL(nextqp), val);
	}

	val = I40E_QINT_TQCTL_CAUSE_ENA_MASK		      |
	      (I40E_TX_ITR << I40E_QINT_TQCTL_ITR_INDX_SHIFT) |
	      (I40E_QUEUE_END_OF_LIST << I40E_QINT_TQCTL_NEXTQ_INDX_SHIFT);

	wr32(hw, I40E_QINT_TQCTL(0), val);
	i40e_flush(hw);
}