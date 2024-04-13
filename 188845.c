static void i40e_set_vsi_rx_mode(struct i40e_vsi *vsi)
{
	if (vsi->netdev)
		i40e_set_rx_mode(vsi->netdev);
}