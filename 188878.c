static void i40e_queue_pair_reset_stats(struct i40e_vsi *vsi, int queue_pair)
{
	memset(&vsi->rx_rings[queue_pair]->rx_stats, 0,
	       sizeof(vsi->rx_rings[queue_pair]->rx_stats));
	memset(&vsi->tx_rings[queue_pair]->stats, 0,
	       sizeof(vsi->tx_rings[queue_pair]->stats));
	if (i40e_enabled_xdp_vsi(vsi)) {
		memset(&vsi->xdp_rings[queue_pair]->stats, 0,
		       sizeof(vsi->xdp_rings[queue_pair]->stats));
	}
}