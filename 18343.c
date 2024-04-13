unsigned long iov_iter_alignment(const struct iov_iter *i)
{
	/* iovec and kvec have identical layouts */
	if (likely(iter_is_iovec(i) || iov_iter_is_kvec(i)))
		return iov_iter_alignment_iovec(i);

	if (iov_iter_is_bvec(i))
		return iov_iter_alignment_bvec(i);

	if (iov_iter_is_pipe(i)) {
		unsigned int p_mask = i->pipe->ring_size - 1;
		size_t size = i->count;

		if (size && i->iov_offset && allocated(&i->pipe->bufs[i->head & p_mask]))
			return size | i->iov_offset;
		return size;
	}

	if (iov_iter_is_xarray(i))
		return (i->xarray_start + i->iov_offset) | i->count;

	return 0;
}