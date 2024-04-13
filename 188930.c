void i40e_vsi_reset_stats(struct i40e_vsi *vsi)
{
	struct rtnl_link_stats64 *ns;
	int i;

	if (!vsi)
		return;

	ns = i40e_get_vsi_stats_struct(vsi);
	memset(ns, 0, sizeof(*ns));
	memset(&vsi->net_stats_offsets, 0, sizeof(vsi->net_stats_offsets));
	memset(&vsi->eth_stats, 0, sizeof(vsi->eth_stats));
	memset(&vsi->eth_stats_offsets, 0, sizeof(vsi->eth_stats_offsets));
	if (vsi->rx_rings && vsi->rx_rings[0]) {
		for (i = 0; i < vsi->num_queue_pairs; i++) {
			memset(&vsi->rx_rings[i]->stats, 0,
			       sizeof(vsi->rx_rings[i]->stats));
			memset(&vsi->rx_rings[i]->rx_stats, 0,
			       sizeof(vsi->rx_rings[i]->rx_stats));
			memset(&vsi->tx_rings[i]->stats, 0,
			       sizeof(vsi->tx_rings[i]->stats));
			memset(&vsi->tx_rings[i]->tx_stats, 0,
			       sizeof(vsi->tx_rings[i]->tx_stats));
		}
	}
	vsi->stat_offsets_loaded = false;
}