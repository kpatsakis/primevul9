static inline void *__prb_previous_block(struct packet_sock *po,
					 struct packet_ring_buffer *rb,
					 int status)
{
	unsigned int previous = prb_previous_blk_num(rb);
	return prb_lookup_block(po, rb, previous, status);
}