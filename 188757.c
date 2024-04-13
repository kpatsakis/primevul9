static void i40e_vsi_free_arrays(struct i40e_vsi *vsi, bool free_qvectors)
{
	/* free the ring and vector containers */
	if (free_qvectors) {
		kfree(vsi->q_vectors);
		vsi->q_vectors = NULL;
	}
	kfree(vsi->tx_rings);
	vsi->tx_rings = NULL;
	vsi->rx_rings = NULL;
	vsi->xdp_rings = NULL;
}