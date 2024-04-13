static inline void prb_freeze_queue(struct kbdq_core *pkc,
				  struct packet_sock *po)
{
	pkc->reset_pending_on_curr_blk = 1;
	po->stats_u.stats3.tp_freeze_q_cnt++;
}