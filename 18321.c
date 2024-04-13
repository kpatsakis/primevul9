int iov_iter_npages(const struct iov_iter *i, int maxpages)
{
	if (unlikely(!i->count))
		return 0;
	/* iovec and kvec have identical layouts */
	if (likely(iter_is_iovec(i) || iov_iter_is_kvec(i)))
		return iov_npages(i, maxpages);
	if (iov_iter_is_bvec(i))
		return bvec_npages(i, maxpages);
	if (iov_iter_is_pipe(i)) {
		unsigned int iter_head;
		int npages;
		size_t off;

		if (!sanity(i))
			return 0;

		data_start(i, &iter_head, &off);
		/* some of this one + all after this one */
		npages = pipe_space_for_user(iter_head, i->pipe->tail, i->pipe);
		return min(npages, maxpages);
	}
	if (iov_iter_is_xarray(i)) {
		unsigned offset = (i->xarray_start + i->iov_offset) % PAGE_SIZE;
		int npages = DIV_ROUND_UP(offset + i->count, PAGE_SIZE);
		return min(npages, maxpages);
	}
	return 0;
}