static size_t copy_mc_pipe_to_iter(const void *addr, size_t bytes,
				struct iov_iter *i)
{
	struct pipe_inode_info *pipe = i->pipe;
	unsigned int p_mask = pipe->ring_size - 1;
	unsigned int i_head;
	size_t n, off, xfer = 0;

	if (!sanity(i))
		return 0;

	n = push_pipe(i, bytes, &i_head, &off);
	while (n) {
		size_t chunk = min_t(size_t, n, PAGE_SIZE - off);
		char *p = kmap_local_page(pipe->bufs[i_head & p_mask].page);
		unsigned long rem;
		rem = copy_mc_to_kernel(p + off, addr + xfer, chunk);
		chunk -= rem;
		kunmap_local(p);
		i->head = i_head;
		i->iov_offset = off + chunk;
		xfer += chunk;
		if (rem)
			break;
		n -= chunk;
		off = 0;
		i_head++;
	}
	i->count -= xfer;
	return xfer;
}