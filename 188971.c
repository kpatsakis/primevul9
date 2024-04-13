static int i40e_channel_config_tx_ring(struct i40e_pf *pf,
				       struct i40e_vsi *vsi,
				       struct i40e_channel *ch)
{
	i40e_status ret;
	int i;
	u8 bw_share[I40E_MAX_TRAFFIC_CLASS] = {0};

	/* Enable ETS TCs with equal BW Share for now across all VSIs */
	for (i = 0; i < I40E_MAX_TRAFFIC_CLASS; i++) {
		if (ch->enabled_tc & BIT(i))
			bw_share[i] = 1;
	}

	/* configure BW for new VSI */
	ret = i40e_channel_config_bw(vsi, ch, bw_share);
	if (ret) {
		dev_info(&vsi->back->pdev->dev,
			 "Failed configuring TC map %d for channel (seid %u)\n",
			 ch->enabled_tc, ch->seid);
		return ret;
	}

	for (i = 0; i < ch->num_queue_pairs; i++) {
		struct i40e_ring *tx_ring, *rx_ring;
		u16 pf_q;

		pf_q = ch->base_queue + i;

		/* Get to TX ring ptr of main VSI, for re-setup TX queue
		 * context
		 */
		tx_ring = vsi->tx_rings[pf_q];
		tx_ring->ch = ch;

		/* Get the RX ring ptr */
		rx_ring = vsi->rx_rings[pf_q];
		rx_ring->ch = ch;
	}

	return 0;
}