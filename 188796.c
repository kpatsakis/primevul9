static void i40e_vsi_free_irq(struct i40e_vsi *vsi)
{
	struct i40e_pf *pf = vsi->back;
	struct i40e_hw *hw = &pf->hw;
	int base = vsi->base_vector;
	u32 val, qp;
	int i;

	if (pf->flags & I40E_FLAG_MSIX_ENABLED) {
		if (!vsi->q_vectors)
			return;

		if (!vsi->irqs_ready)
			return;

		vsi->irqs_ready = false;
		for (i = 0; i < vsi->num_q_vectors; i++) {
			int irq_num;
			u16 vector;

			vector = i + base;
			irq_num = pf->msix_entries[vector].vector;

			/* free only the irqs that were actually requested */
			if (!vsi->q_vectors[i] ||
			    !vsi->q_vectors[i]->num_ringpairs)
				continue;

			/* clear the affinity notifier in the IRQ descriptor */
			irq_set_affinity_notifier(irq_num, NULL);
			/* remove our suggested affinity mask for this IRQ */
			irq_set_affinity_hint(irq_num, NULL);
			synchronize_irq(irq_num);
			free_irq(irq_num, vsi->q_vectors[i]);

			/* Tear down the interrupt queue link list
			 *
			 * We know that they come in pairs and always
			 * the Rx first, then the Tx.  To clear the
			 * link list, stick the EOL value into the
			 * next_q field of the registers.
			 */
			val = rd32(hw, I40E_PFINT_LNKLSTN(vector - 1));
			qp = (val & I40E_PFINT_LNKLSTN_FIRSTQ_INDX_MASK)
				>> I40E_PFINT_LNKLSTN_FIRSTQ_INDX_SHIFT;
			val |= I40E_QUEUE_END_OF_LIST
				<< I40E_PFINT_LNKLSTN_FIRSTQ_INDX_SHIFT;
			wr32(hw, I40E_PFINT_LNKLSTN(vector - 1), val);

			while (qp != I40E_QUEUE_END_OF_LIST) {
				u32 next;

				val = rd32(hw, I40E_QINT_RQCTL(qp));

				val &= ~(I40E_QINT_RQCTL_MSIX_INDX_MASK  |
					 I40E_QINT_RQCTL_MSIX0_INDX_MASK |
					 I40E_QINT_RQCTL_CAUSE_ENA_MASK  |
					 I40E_QINT_RQCTL_INTEVENT_MASK);

				val |= (I40E_QINT_RQCTL_ITR_INDX_MASK |
					 I40E_QINT_RQCTL_NEXTQ_INDX_MASK);

				wr32(hw, I40E_QINT_RQCTL(qp), val);

				val = rd32(hw, I40E_QINT_TQCTL(qp));

				next = (val & I40E_QINT_TQCTL_NEXTQ_INDX_MASK)
					>> I40E_QINT_TQCTL_NEXTQ_INDX_SHIFT;

				val &= ~(I40E_QINT_TQCTL_MSIX_INDX_MASK  |
					 I40E_QINT_TQCTL_MSIX0_INDX_MASK |
					 I40E_QINT_TQCTL_CAUSE_ENA_MASK  |
					 I40E_QINT_TQCTL_INTEVENT_MASK);

				val |= (I40E_QINT_TQCTL_ITR_INDX_MASK |
					 I40E_QINT_TQCTL_NEXTQ_INDX_MASK);

				wr32(hw, I40E_QINT_TQCTL(qp), val);
				qp = next;
			}
		}
	} else {
		free_irq(pf->pdev->irq, pf);

		val = rd32(hw, I40E_PFINT_LNKLST0);
		qp = (val & I40E_PFINT_LNKLSTN_FIRSTQ_INDX_MASK)
			>> I40E_PFINT_LNKLSTN_FIRSTQ_INDX_SHIFT;
		val |= I40E_QUEUE_END_OF_LIST
			<< I40E_PFINT_LNKLST0_FIRSTQ_INDX_SHIFT;
		wr32(hw, I40E_PFINT_LNKLST0, val);

		val = rd32(hw, I40E_QINT_RQCTL(qp));
		val &= ~(I40E_QINT_RQCTL_MSIX_INDX_MASK  |
			 I40E_QINT_RQCTL_MSIX0_INDX_MASK |
			 I40E_QINT_RQCTL_CAUSE_ENA_MASK  |
			 I40E_QINT_RQCTL_INTEVENT_MASK);

		val |= (I40E_QINT_RQCTL_ITR_INDX_MASK |
			I40E_QINT_RQCTL_NEXTQ_INDX_MASK);

		wr32(hw, I40E_QINT_RQCTL(qp), val);

		val = rd32(hw, I40E_QINT_TQCTL(qp));

		val &= ~(I40E_QINT_TQCTL_MSIX_INDX_MASK  |
			 I40E_QINT_TQCTL_MSIX0_INDX_MASK |
			 I40E_QINT_TQCTL_CAUSE_ENA_MASK  |
			 I40E_QINT_TQCTL_INTEVENT_MASK);

		val |= (I40E_QINT_TQCTL_ITR_INDX_MASK |
			I40E_QINT_TQCTL_NEXTQ_INDX_MASK);

		wr32(hw, I40E_QINT_TQCTL(qp), val);
	}
}