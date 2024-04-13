static void iov_iter_bvec_advance(struct iov_iter *i, size_t size)
{
	struct bvec_iter bi;

	bi.bi_size = i->count;
	bi.bi_bvec_done = i->iov_offset;
	bi.bi_idx = 0;
	bvec_iter_advance(i->bvec, &bi, size);

	i->bvec += bi.bi_idx;
	i->nr_segs -= bi.bi_idx;
	i->count = bi.bi_size;
	i->iov_offset = bi.bi_bvec_done;
}