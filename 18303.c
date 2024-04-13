ssize_t iov_iter_get_pages_alloc(struct iov_iter *i,
		   struct page ***pages, size_t maxsize,
		   size_t *start)
{
	struct page **p;
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

		addr = first_iovec_segment(i, &len, start, maxsize, ~0U);
		n = DIV_ROUND_UP(len, PAGE_SIZE);
		p = get_pages_array(n);
		if (!p)
			return -ENOMEM;
		res = get_user_pages_fast(addr, n, gup_flags, p);
		if (unlikely(res <= 0)) {
			kvfree(p);
			*pages = NULL;
			return res;
		}
		*pages = p;
		return (res == n ? len : res * PAGE_SIZE) - *start;
	}
	if (iov_iter_is_bvec(i)) {
		struct page *page;

		page = first_bvec_segment(i, &len, start, maxsize, ~0U);
		n = DIV_ROUND_UP(len, PAGE_SIZE);
		*pages = p = get_pages_array(n);
		if (!p)
			return -ENOMEM;
		while (n--)
			get_page(*p++ = page++);
		return len - *start;
	}
	if (iov_iter_is_pipe(i))
		return pipe_get_pages_alloc(i, pages, maxsize, start);
	if (iov_iter_is_xarray(i))
		return iter_xarray_get_pages_alloc(i, pages, maxsize, start);
	return -EFAULT;
}