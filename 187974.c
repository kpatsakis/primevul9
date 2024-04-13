static inline void *prb_lookup_block(struct packet_sock *po,
				     struct packet_ring_buffer *rb,
				     unsigned int previous,
				     int status)
{
	struct kbdq_core *pkc  = GET_PBDQC_FROM_RB(rb);
	struct block_desc *pbd = GET_PBLOCK_DESC(pkc, previous);

	if (status != BLOCK_STATUS(pbd))
		return NULL;
	return pbd;
}