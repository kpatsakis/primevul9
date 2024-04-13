static void i40e_quiesce_vsi(struct i40e_vsi *vsi)
{
	if (test_bit(__I40E_VSI_DOWN, vsi->state))
		return;

	set_bit(__I40E_VSI_NEEDS_RESTART, vsi->state);
	if (vsi->netdev && netif_running(vsi->netdev))
		vsi->netdev->netdev_ops->ndo_stop(vsi->netdev);
	else
		i40e_vsi_close(vsi);
}