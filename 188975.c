static void i40e_vsi_config_dcb_rings(struct i40e_vsi *vsi)
{
	struct i40e_ring *tx_ring, *rx_ring;
	u16 qoffset, qcount;
	int i, n;

	if (!(vsi->back->flags & I40E_FLAG_DCB_ENABLED)) {
		/* Reset the TC information */
		for (i = 0; i < vsi->num_queue_pairs; i++) {
			rx_ring = vsi->rx_rings[i];
			tx_ring = vsi->tx_rings[i];
			rx_ring->dcb_tc = 0;
			tx_ring->dcb_tc = 0;
		}
		return;
	}

	for (n = 0; n < I40E_MAX_TRAFFIC_CLASS; n++) {
		if (!(vsi->tc_config.enabled_tc & BIT_ULL(n)))
			continue;

		qoffset = vsi->tc_config.tc_info[n].qoffset;
		qcount = vsi->tc_config.tc_info[n].qcount;
		for (i = qoffset; i < (qoffset + qcount); i++) {
			rx_ring = vsi->rx_rings[i];
			tx_ring = vsi->tx_rings[i];
			rx_ring->dcb_tc = n;
			tx_ring->dcb_tc = n;
		}
	}
}