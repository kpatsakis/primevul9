ssize_t iov_iter_get_pages(struct iov_iter *i,
		   struct page **pages, size_t maxsize, unsigned maxpages,
		   size_t *start)
{
	size_t len;
	int n, res;

	if (maxsize > i->count)
		maxsize = i->count;
	if (!maxsize)
		return 0;

	if (likely(iter_is_iovec(i))) {
		unsigned int gup_flags = 0;
		unsigned long addr;

		if (iov_iter_rw(i) != WRITE)
			gup_flags |= FOLL_WRITE;
		if (i->nofault)
			gup_flags |= FOLL_NOFAULT;

		addr = first_iovec_segment(i, &len, start, maxsize, maxpages);
		n = DIV_ROUND_UP(len, PAGE_SIZE);
		res = get_user_pages_fast(addr, n, gup_flags, pages);
		if (unlikely(res <= 0))
			return res;
		return (res == n ? len : res * PAGE_SIZE) - *start;
	}
	if (iov_iter_is_bvec(i)) {
		struct page *page;

		page = first_bvec_segment(i, &len, start, maxsize, maxpages);
		n = DIV_ROUND_UP(len, PAGE_SIZE);
		while (n--)
			get_page(*pages++ = page++);
		return len - *start;
	}
	if (iov_iter_is_pipe(i))
		return pipe_get_pages(i, pages, maxsize, maxpages, start);
	if (iov_iter_is_xarray(i))
		return iter_xarray_get_pages(i, pages, maxsize, maxpages, start);
	return -EFAULT;
}