static void i40e_get_netdev_stats_struct_tx(struct i40e_ring *ring,
					    struct rtnl_link_stats64 *stats)
{
	u64 bytes, packets;
	unsigned int start;

	do {
		start = u64_stats_fetch_begin_irq(&ring->syncp);
		packets = ring->stats.packets;
		bytes   = ring->stats.bytes;
	} while (u64_stats_fetch_retry_irq(&ring->syncp, start));

	stats->tx_packets += packets;
	stats->tx_bytes   += bytes;
}