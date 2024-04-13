static inline ssize_t __pipe_get_pages(struct iov_iter *i,
				size_t maxsize,
				struct page **pages,
				int iter_head,
				size_t *start)
{
	struct pipe_inode_info *pipe = i->pipe;
	unsigned int p_mask = pipe->ring_size - 1;
	ssize_t n = push_pipe(i, maxsize, &iter_head, start);
	if (!n)
		return -EFAULT;

	maxsize = n;
	n += *start;
	while (n > 0) {
		get_page(*pages++ = pipe->bufs[iter_head & p_mask].page);
		iter_head++;
		n -= PAGE_SIZE;
	}

	return maxsize;
}