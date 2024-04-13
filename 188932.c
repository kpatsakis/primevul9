static void i40e_napi_disable_all(struct i40e_vsi *vsi)
{
	int q_idx;

	if (!vsi->netdev)
		return;

	for (q_idx = 0; q_idx < vsi->num_q_vectors; q_idx++) {
		struct i40e_q_vector *q_vector = vsi->q_vectors[q_idx];

		if (q_vector->rx.ring || q_vector->tx.ring)
			napi_disable(&q_vector->napi);
	}
}