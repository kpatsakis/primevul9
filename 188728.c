static void i40e_reset_ch_rings(struct i40e_vsi *vsi, struct i40e_channel *ch)
{
	struct i40e_ring *tx_ring, *rx_ring;
	u16 pf_q;
	int i;

	for (i = 0; i < ch->num_queue_pairs; i++) {
		pf_q = ch->base_queue + i;
		tx_ring = vsi->tx_rings[pf_q];
		tx_ring->ch = NULL;
		rx_ring = vsi->rx_rings[pf_q];
		rx_ring->ch = NULL;
	}
}