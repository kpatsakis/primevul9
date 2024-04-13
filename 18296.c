static size_t copy_pipe_to_iter(const void *addr, size_t bytes,
				struct iov_iter *i)
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
		memcpy_to_page(pipe->bufs[i_head & p_mask].page, off, addr, chunk);
		i->head = i_head;
		i->iov_offset = off + chunk;
		n -= chunk;
		addr += chunk;
		off = 0;
		i_head++;
	} while (n);
	i->count -= bytes;
	return bytes;
}