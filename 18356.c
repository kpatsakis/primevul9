static void iov_iter_iovec_advance(struct iov_iter *i, size_t size)
{
	const struct iovec *iov, *end;

	if (!i->count)
		return;
	i->count -= size;

	size += i->iov_offset; // from beginning of current segment
	for (iov = i->iov, end = iov + i->nr_segs; iov < end; iov++) {
		if (likely(size < iov->iov_len))
			break;
		size -= iov->iov_len;
	}
	i->iov_offset = size;
	i->nr_segs -= iov - i->iov;
	i->iov = iov;
}