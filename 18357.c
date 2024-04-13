void iov_iter_discard(struct iov_iter *i, unsigned int direction, size_t count)
{
	BUG_ON(direction != READ);
	*i = (struct iov_iter){
		.iter_type = ITER_DISCARD,
		.data_source = false,
		.count = count,
		.iov_offset = 0
	};
}