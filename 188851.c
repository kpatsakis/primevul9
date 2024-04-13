static void i40e_queue_pair_toggle_napi(struct i40e_vsi *vsi, int queue_pair,
					bool enable)
{
	struct i40e_ring *rxr = vsi->rx_rings[queue_pair];
	struct i40e_q_vector *q_vector = rxr->q_vector;

	if (!vsi->netdev)
		return;

	/* All rings in a qp belong to the same qvector. */
	if (q_vector->rx.ring || q_vector->tx.ring) {
		if (enable)
			napi_enable(&q_vector->napi);
		else
			napi_disable(&q_vector->napi);
	}
}