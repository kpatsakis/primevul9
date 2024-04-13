static inline int prb_previous_blk_num(struct packet_ring_buffer *rb)
{
	unsigned int prev;
	if (rb->prb_bdqc.kactive_blk_num)
		prev = rb->prb_bdqc.kactive_blk_num-1;
	else
		prev = rb->prb_bdqc.knum_blocks-1;
	return prev;
}