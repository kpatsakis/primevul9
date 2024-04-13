static void prb_setup_retire_blk_timer(struct packet_sock *po, int tx_ring)
{
	struct kbdq_core *pkc;

	if (tx_ring)
		BUG();

	pkc = tx_ring ? &po->tx_ring.prb_bdqc : &po->rx_ring.prb_bdqc;
	prb_init_blk_timer(po, pkc, prb_retire_rx_blk_timer_expired);
}