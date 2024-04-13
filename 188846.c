static void i40e_update_vsi_stats(struct i40e_vsi *vsi)
{
	struct i40e_pf *pf = vsi->back;
	struct rtnl_link_stats64 *ons;
	struct rtnl_link_stats64 *ns;   /* netdev stats */
	struct i40e_eth_stats *oes;
	struct i40e_eth_stats *es;     /* device's eth stats */
	u32 tx_restart, tx_busy;
	struct i40e_ring *p;
	u32 rx_page, rx_buf;
	u64 bytes, packets;
	unsigned int start;
	u64 tx_linearize;
	u64 tx_force_wb;
	u64 rx_p, rx_b;
	u64 tx_p, tx_b;
	u16 q;

	if (test_bit(__I40E_VSI_DOWN, vsi->state) ||
	    test_bit(__I40E_CONFIG_BUSY, pf->state))
		return;

	ns = i40e_get_vsi_stats_struct(vsi);
	ons = &vsi->net_stats_offsets;
	es = &vsi->eth_stats;
	oes = &vsi->eth_stats_offsets;

	/* Gather up the netdev and vsi stats that the driver collects
	 * on the fly during packet processing
	 */
	rx_b = rx_p = 0;
	tx_b = tx_p = 0;
	tx_restart = tx_busy = tx_linearize = tx_force_wb = 0;
	rx_page = 0;
	rx_buf = 0;
	rcu_read_lock();
	for (q = 0; q < vsi->num_queue_pairs; q++) {
		/* locate Tx ring */
		p = READ_ONCE(vsi->tx_rings[q]);

		do {
			start = u64_stats_fetch_begin_irq(&p->syncp);
			packets = p->stats.packets;
			bytes = p->stats.bytes;
		} while (u64_stats_fetch_retry_irq(&p->syncp, start));
		tx_b += bytes;
		tx_p += packets;
		tx_restart += p->tx_stats.restart_queue;
		tx_busy += p->tx_stats.tx_busy;
		tx_linearize += p->tx_stats.tx_linearize;
		tx_force_wb += p->tx_stats.tx_force_wb;

		/* Rx queue is part of the same block as Tx queue */
		p = &p[1];
		do {
			start = u64_stats_fetch_begin_irq(&p->syncp);
			packets = p->stats.packets;
			bytes = p->stats.bytes;
		} while (u64_stats_fetch_retry_irq(&p->syncp, start));
		rx_b += bytes;
		rx_p += packets;
		rx_buf += p->rx_stats.alloc_buff_failed;
		rx_page += p->rx_stats.alloc_page_failed;
	}
	rcu_read_unlock();
	vsi->tx_restart = tx_restart;
	vsi->tx_busy = tx_busy;
	vsi->tx_linearize = tx_linearize;
	vsi->tx_force_wb = tx_force_wb;
	vsi->rx_page_failed = rx_page;
	vsi->rx_buf_failed = rx_buf;

	ns->rx_packets = rx_p;
	ns->rx_bytes = rx_b;
	ns->tx_packets = tx_p;
	ns->tx_bytes = tx_b;

	/* update netdev stats from eth stats */
	i40e_update_eth_stats(vsi);
	ons->tx_errors = oes->tx_errors;
	ns->tx_errors = es->tx_errors;
	ons->multicast = oes->rx_multicast;
	ns->multicast = es->rx_multicast;
	ons->rx_dropped = oes->rx_discards;
	ns->rx_dropped = es->rx_discards;
	ons->tx_dropped = oes->tx_discards;
	ns->tx_dropped = es->tx_discards;

	/* pull in a couple PF stats if this is the main vsi */
	if (vsi == pf->vsi[pf->lan_vsi]) {
		ns->rx_crc_errors = pf->stats.crc_errors;
		ns->rx_errors = pf->stats.crc_errors + pf->stats.illegal_bytes;
		ns->rx_length_errors = pf->stats.rx_length_errors;
	}
}