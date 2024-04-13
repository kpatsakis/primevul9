static ssize_t pipe_get_pages_alloc(struct iov_iter *i,
		   struct page ***pages, size_t maxsize,
		   size_t *start)
{
	struct page **p;
	unsigned int iter_head, npages;
	ssize_t n;

	if (!sanity(i))
		return -EFAULT;

	data_start(i, &iter_head, start);
	/* Amount of free space: some of this one + all after this one */
	npages = pipe_space_for_user(iter_head, i->pipe->tail, i->pipe);
	n = npages * PAGE_SIZE - *start;
	if (maxsize > n)
		maxsize = n;
	else
		npages = DIV_ROUND_UP(maxsize + *start, PAGE_SIZE);
	p = get_pages_array(npages);
	if (!p)
		return -ENOMEM;
	n = __pipe_get_pages(i, maxsize, p, iter_head, start);
	if (n > 0)
		*pages = p;
	else
		kvfree(p);
	return n;
}