static void i40e_get_netdev_stats_struct(struct net_device *netdev,
				  struct rtnl_link_stats64 *stats)
{
	struct i40e_netdev_priv *np = netdev_priv(netdev);
	struct i40e_vsi *vsi = np->vsi;
	struct rtnl_link_stats64 *vsi_stats = i40e_get_vsi_stats_struct(vsi);
	struct i40e_ring *ring;
	int i;

	if (test_bit(__I40E_VSI_DOWN, vsi->state))
		return;

	if (!vsi->tx_rings)
		return;

	rcu_read_lock();
	for (i = 0; i < vsi->num_queue_pairs; i++) {
		u64 bytes, packets;
		unsigned int start;

		ring = READ_ONCE(vsi->tx_rings[i]);
		if (!ring)
			continue;
		i40e_get_netdev_stats_struct_tx(ring, stats);

		if (i40e_enabled_xdp_vsi(vsi)) {
			ring++;
			i40e_get_netdev_stats_struct_tx(ring, stats);
		}

		ring++;
		do {
			start   = u64_stats_fetch_begin_irq(&ring->syncp);
			packets = ring->stats.packets;
			bytes   = ring->stats.bytes;
		} while (u64_stats_fetch_retry_irq(&ring->syncp, start));

		stats->rx_packets += packets;
		stats->rx_bytes   += bytes;

	}
	rcu_read_unlock();

	/* following stats updated by i40e_watchdog_subtask() */
	stats->multicast	= vsi_stats->multicast;
	stats->tx_errors	= vsi_stats->tx_errors;
	stats->tx_dropped	= vsi_stats->tx_dropped;
	stats->rx_errors	= vsi_stats->rx_errors;
	stats->rx_dropped	= vsi_stats->rx_dropped;
	stats->rx_crc_errors	= vsi_stats->rx_crc_errors;
	stats->rx_length_errors	= vsi_stats->rx_length_errors;
}