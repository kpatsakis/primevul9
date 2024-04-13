static int i40e_vsi_setup_tx_resources(struct i40e_vsi *vsi)
{
	int i, err = 0;

	for (i = 0; i < vsi->num_queue_pairs && !err; i++)
		err = i40e_setup_tx_descriptors(vsi->tx_rings[i]);

	if (!i40e_enabled_xdp_vsi(vsi))
		return err;

	for (i = 0; i < vsi->num_queue_pairs && !err; i++)
		err = i40e_setup_tx_descriptors(vsi->xdp_rings[i]);

	return err;
}