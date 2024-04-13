static int i40e_vsi_configure_tx(struct i40e_vsi *vsi)
{
	int err = 0;
	u16 i;

	for (i = 0; (i < vsi->num_queue_pairs) && !err; i++)
		err = i40e_configure_tx_ring(vsi->tx_rings[i]);

	if (err || !i40e_enabled_xdp_vsi(vsi))
		return err;

	for (i = 0; (i < vsi->num_queue_pairs) && !err; i++)
		err = i40e_configure_tx_ring(vsi->xdp_rings[i]);

	return err;
}