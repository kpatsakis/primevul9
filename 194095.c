static inline bool bvec_gap_to_prev(struct request_queue *q,
		struct bio_vec *bprv, unsigned int offset)
{
	if (!queue_virt_boundary(q))
		return false;
	return __bvec_gap_to_prev(q, bprv, offset);
}