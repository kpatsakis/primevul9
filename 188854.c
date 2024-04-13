static int i40e_vsi_alloc_q_vector(struct i40e_vsi *vsi, int v_idx, int cpu)
{
	struct i40e_q_vector *q_vector;

	/* allocate q_vector */
	q_vector = kzalloc(sizeof(struct i40e_q_vector), GFP_KERNEL);
	if (!q_vector)
		return -ENOMEM;

	q_vector->vsi = vsi;
	q_vector->v_idx = v_idx;
	cpumask_copy(&q_vector->affinity_mask, cpu_possible_mask);

	if (vsi->netdev)
		netif_napi_add(vsi->netdev, &q_vector->napi,
			       i40e_napi_poll, NAPI_POLL_WEIGHT);

	/* tie q_vector and vsi together */
	vsi->q_vectors[v_idx] = q_vector;

	return 0;
}