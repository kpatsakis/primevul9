static void prb_shutdown_retire_blk_timer(struct packet_sock *po,
		int tx_ring,
		struct sk_buff_head *rb_queue)
{
	struct kbdq_core *pkc;

	pkc = tx_ring ? &po->tx_ring.prb_bdqc : &po->rx_ring.prb_bdqc;

	spin_lock(&rb_queue->lock);
	pkc->delete_blk_timer = 1;
	spin_unlock(&rb_queue->lock);

	prb_del_retire_blk_timer(pkc);
}