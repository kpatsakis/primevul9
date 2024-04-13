static inline void prb_clear_blk_fill_status(struct packet_ring_buffer *rb)
{
	struct kbdq_core *pkc  = GET_PBDQC_FROM_RB(rb);
	atomic_dec(&pkc->blk_fill_in_prog);
}