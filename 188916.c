void i40e_down(struct i40e_vsi *vsi)
{
	int i;

	/* It is assumed that the caller of this function
	 * sets the vsi->state __I40E_VSI_DOWN bit.
	 */
	if (vsi->netdev) {
		netif_carrier_off(vsi->netdev);
		netif_tx_disable(vsi->netdev);
	}
	i40e_vsi_disable_irq(vsi);
	i40e_vsi_stop_rings(vsi);
	if (vsi->type == I40E_VSI_MAIN &&
	    vsi->back->flags & I40E_FLAG_LINK_DOWN_ON_CLOSE_ENABLED)
		i40e_force_link_state(vsi->back, false);
	i40e_napi_disable_all(vsi);

	for (i = 0; i < vsi->num_queue_pairs; i++) {
		i40e_clean_tx_ring(vsi->tx_rings[i]);
		if (i40e_enabled_xdp_vsi(vsi)) {
			/* Make sure that in-progress ndo_xdp_xmit
			 * calls are completed.
			 */
			synchronize_rcu();
			i40e_clean_tx_ring(vsi->xdp_rings[i]);
		}
		i40e_clean_rx_ring(vsi->rx_rings[i]);
	}

}