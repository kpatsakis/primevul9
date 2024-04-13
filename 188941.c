static void i40e_unquiesce_vsi(struct i40e_vsi *vsi)
{
	if (!test_and_clear_bit(__I40E_VSI_NEEDS_RESTART, vsi->state))
		return;

	if (vsi->netdev && netif_running(vsi->netdev))
		vsi->netdev->netdev_ops->ndo_open(vsi->netdev);
	else
		i40e_vsi_open(vsi);   /* this clears the DOWN bit */
}