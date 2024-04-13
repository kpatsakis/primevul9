static void i40e_queue_pair_clean_rings(struct i40e_vsi *vsi, int queue_pair)
{
	i40e_clean_tx_ring(vsi->tx_rings[queue_pair]);
	if (i40e_enabled_xdp_vsi(vsi)) {
		/* Make sure that in-progress ndo_xdp_xmit calls are
		 * completed.
		 */
		synchronize_rcu();
		i40e_clean_tx_ring(vsi->xdp_rings[queue_pair]);
	}
	i40e_clean_rx_ring(vsi->rx_rings[queue_pair]);
}