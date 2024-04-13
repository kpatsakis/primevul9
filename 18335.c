void iov_iter_bvec(struct iov_iter *i, unsigned int direction,
			const struct bio_vec *bvec, unsigned long nr_segs,
			size_t count)
{
	WARN_ON(direction & ~(READ | WRITE));
	*i = (struct iov_iter){
		.iter_type = ITER_BVEC,
		.data_source = direction,
		.bvec = bvec,
		.nr_segs = nr_segs,
		.iov_offset = 0,
		.count = count
	};
}