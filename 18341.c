static ssize_t pipe_get_pages(struct iov_iter *i,
		   struct page **pages, size_t maxsize, unsigned maxpages,
		   size_t *start)
{
	unsigned int iter_head, npages;
	size_t capacity;

	if (!sanity(i))
		return -EFAULT;

	data_start(i, &iter_head, start);
	/* Amount of free space: some of this one + all after this one */
	npages = pipe_space_for_user(iter_head, i->pipe->tail, i->pipe);
	capacity = min(npages, maxpages) * PAGE_SIZE - *start;

	return __pipe_get_pages(i, min(maxsize, capacity), pages, iter_head, start);
}