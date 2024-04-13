static int i40e_vsi_alloc_arrays(struct i40e_vsi *vsi, bool alloc_qvectors)
{
	struct i40e_ring **next_rings;
	int size;
	int ret = 0;

	/* allocate memory for both Tx, XDP Tx and Rx ring pointers */
	size = sizeof(struct i40e_ring *) * vsi->alloc_queue_pairs *
	       (i40e_enabled_xdp_vsi(vsi) ? 3 : 2);
	vsi->tx_rings = kzalloc(size, GFP_KERNEL);
	if (!vsi->tx_rings)
		return -ENOMEM;
	next_rings = vsi->tx_rings + vsi->alloc_queue_pairs;
	if (i40e_enabled_xdp_vsi(vsi)) {
		vsi->xdp_rings = next_rings;
		next_rings += vsi->alloc_queue_pairs;
	}
	vsi->rx_rings = next_rings;

	if (alloc_qvectors) {
		/* allocate memory for q_vector pointers */
		size = sizeof(struct i40e_q_vector *) * vsi->num_q_vectors;
		vsi->q_vectors = kzalloc(size, GFP_KERNEL);
		if (!vsi->q_vectors) {
			ret = -ENOMEM;
			goto err_vectors;
		}
	}
	return ret;

err_vectors:
	kfree(vsi->tx_rings);
	return ret;
}