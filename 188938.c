static void i40e_vsi_free_rx_resources(struct i40e_vsi *vsi)
{
	int i;

	if (!vsi->rx_rings)
		return;

	for (i = 0; i < vsi->num_queue_pairs; i++)
		if (vsi->rx_rings[i] && vsi->rx_rings[i]->desc)
			i40e_free_rx_resources(vsi->rx_rings[i]);
}