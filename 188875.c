static void i40e_free_q_vector(struct i40e_vsi *vsi, int v_idx)
{
	struct i40e_q_vector *q_vector = vsi->q_vectors[v_idx];
	struct i40e_ring *ring;

	if (!q_vector)
		return;

	/* disassociate q_vector from rings */
	i40e_for_each_ring(ring, q_vector->tx)
		ring->q_vector = NULL;

	i40e_for_each_ring(ring, q_vector->rx)
		ring->q_vector = NULL;

	/* only VSI w/ an associated netdev is set up w/ NAPI */
	if (vsi->netdev)
		netif_napi_del(&q_vector->napi);

	vsi->q_vectors[v_idx] = NULL;

	kfree_rcu(q_vector, rcu);
}