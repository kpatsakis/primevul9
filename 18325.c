static size_t csum_and_copy_to_pipe_iter(const void *addr, size_t bytes,
					 struct iov_iter *i, __wsum *sump)
{
	struct pipe_inode_info *pipe = i->pipe;
	unsigned int p_mask = pipe->ring_size - 1;
	__wsum sum = *sump;
	size_t off = 0;
	unsigned int i_head;
	size_t r;

	if (!sanity(i))
		return 0;

	bytes = push_pipe(i, bytes, &i_head, &r);
	while (bytes) {
		size_t chunk = min_t(size_t, bytes, PAGE_SIZE - r);
		char *p = kmap_local_page(pipe->bufs[i_head & p_mask].page);
		sum = csum_and_memcpy(p + r, addr + off, chunk, sum, off);
		kunmap_local(p);
		i->head = i_head;
		i->iov_offset = r + chunk;
		bytes -= chunk;
		off += chunk;
		r = 0;
		i_head++;
	}
	*sump = sum;
	i->count -= off;
	return off;
}