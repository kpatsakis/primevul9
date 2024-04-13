static void __blk_rq_prep_clone(struct request *dst, struct request *src)
{
	dst->__sector = blk_rq_pos(src);
	dst->__data_len = blk_rq_bytes(src);
	if (src->rq_flags & RQF_SPECIAL_PAYLOAD) {
		dst->rq_flags |= RQF_SPECIAL_PAYLOAD;
		dst->special_vec = src->special_vec;
	}
	dst->nr_phys_segments = src->nr_phys_segments;
	dst->ioprio = src->ioprio;
	dst->extra_len = src->extra_len;
}