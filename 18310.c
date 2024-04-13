void iov_iter_kvec(struct iov_iter *i, unsigned int direction,
			const struct kvec *kvec, unsigned long nr_segs,
			size_t count)
{
	WARN_ON(direction & ~(READ | WRITE));
	*i = (struct iov_iter){
		.iter_type = ITER_KVEC,
		.data_source = direction,
		.kvec = kvec,
		.nr_segs = nr_segs,
		.iov_offset = 0,
		.count = count
	};
}