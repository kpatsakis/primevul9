static void i40e_map_vector_to_qp(struct i40e_vsi *vsi, int v_idx, int qp_idx)
{
	struct i40e_q_vector *q_vector = vsi->q_vectors[v_idx];
	struct i40e_ring *tx_ring = vsi->tx_rings[qp_idx];
	struct i40e_ring *rx_ring = vsi->rx_rings[qp_idx];

	tx_ring->q_vector = q_vector;
	tx_ring->next = q_vector->tx.ring;
	q_vector->tx.ring = tx_ring;
	q_vector->tx.count++;

	/* Place XDP Tx ring in the same q_vector ring list as regular Tx */
	if (i40e_enabled_xdp_vsi(vsi)) {
		struct i40e_ring *xdp_ring = vsi->xdp_rings[qp_idx];

		xdp_ring->q_vector = q_vector;
		xdp_ring->next = q_vector->tx.ring;
		q_vector->tx.ring = xdp_ring;
		q_vector->tx.count++;
	}

	rx_ring->q_vector = q_vector;
	rx_ring->next = q_vector->rx.ring;
	q_vector->rx.ring = rx_ring;
	q_vector->rx.count++;
}