static size_t pipe_zero(size_t bytes, struct iov_iter *i)
{
	struct pipe_inode_info *pipe = i->pipe;
	unsigned int p_mask = pipe->ring_size - 1;
	unsigned int i_head;
	size_t n, off;

	if (!sanity(i))
		return 0;

	bytes = n = push_pipe(i, bytes, &i_head, &off);
	if (unlikely(!n))
		return 0;

	do {
		size_t chunk = min_t(size_t, n, PAGE_SIZE - off);
		char *p = kmap_local_page(pipe->bufs[i_head & p_mask].page);
		memset(p + off, 0, chunk);
		kunmap_local(p);
		i->head = i_head;
		i->iov_offset = off + chunk;
		n -= chunk;
		off = 0;
		i_head++;
	} while (n);
	i->count -= bytes;
	return bytes;
}