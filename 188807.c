static irqreturn_t i40e_fdir_clean_ring(int irq, void *data)
{
	struct i40e_q_vector *q_vector = data;
	struct i40e_vsi *vsi;

	if (!q_vector->tx.ring)
		return IRQ_HANDLED;

	vsi = q_vector->tx.ring->vsi;
	i40e_clean_fdir_tx_irq(q_vector->tx.ring, vsi->work_limit);

	return IRQ_HANDLED;
}