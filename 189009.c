static void i40e_vsi_close(struct i40e_vsi *vsi)
{
	struct i40e_pf *pf = vsi->back;
	if (!test_and_set_bit(__I40E_VSI_DOWN, vsi->state))
		i40e_down(vsi);
	i40e_vsi_free_irq(vsi);
	i40e_vsi_free_tx_resources(vsi);
	i40e_vsi_free_rx_resources(vsi);
	vsi->current_netdev_flags = 0;
	set_bit(__I40E_CLIENT_SERVICE_REQUESTED, pf->state);
	if (test_bit(__I40E_RESET_RECOVERY_PENDING, pf->state))
		set_bit(__I40E_CLIENT_RESET, pf->state);
}