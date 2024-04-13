static void i40e_vsi_free_tx_resources(struct i40e_vsi *vsi)
{
	int i;

	if (vsi->tx_rings) {
		for (i = 0; i < vsi->num_queue_pairs; i++)
			if (vsi->tx_rings[i] && vsi->tx_rings[i]->desc)
				i40e_free_tx_resources(vsi->tx_rings[i]);
	}

	if (vsi->xdp_rings) {
		for (i = 0; i < vsi->num_queue_pairs; i++)
			if (vsi->xdp_rings[i] && vsi->xdp_rings[i]->desc)
				i40e_free_tx_resources(vsi->xdp_rings[i]);
	}
}