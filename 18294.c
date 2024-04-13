void iov_iter_advance(struct iov_iter *i, size_t size)
{
	if (unlikely(i->count < size))
		size = i->count;
	if (likely(iter_is_iovec(i) || iov_iter_is_kvec(i))) {
		/* iovec and kvec have identical layouts */
		iov_iter_iovec_advance(i, size);
	} else if (iov_iter_is_bvec(i)) {
		iov_iter_bvec_advance(i, size);
	} else if (iov_iter_is_pipe(i)) {
		pipe_advance(i, size);
	} else if (unlikely(iov_iter_is_xarray(i))) {
		i->iov_offset += size;
		i->count -= size;
	} else if (iov_iter_is_discard(i)) {
		i->count -= size;
	}
}