static bool sanity(const struct iov_iter *i)
{
	struct pipe_inode_info *pipe = i->pipe;
	unsigned int p_head = pipe->head;
	unsigned int p_tail = pipe->tail;
	unsigned int p_mask = pipe->ring_size - 1;
	unsigned int p_occupancy = pipe_occupancy(p_head, p_tail);
	unsigned int i_head = i->head;
	unsigned int idx;

	if (i->iov_offset) {
		struct pipe_buffer *p;
		if (unlikely(p_occupancy == 0))
			goto Bad;	// pipe must be non-empty
		if (unlikely(i_head != p_head - 1))
			goto Bad;	// must be at the last buffer...

		p = &pipe->bufs[i_head & p_mask];
		if (unlikely(p->offset + p->len != i->iov_offset))
			goto Bad;	// ... at the end of segment
	} else {
		if (i_head != p_head)
			goto Bad;	// must be right after the last buffer
	}
	return true;
Bad:
	printk(KERN_ERR "idx = %d, offset = %zd\n", i_head, i->iov_offset);
	printk(KERN_ERR "head = %d, tail = %d, buffers = %d\n",
			p_head, p_tail, pipe->ring_size);
	for (idx = 0; idx < pipe->ring_size; idx++)
		printk(KERN_ERR "[%p %p %d %d]\n",
			pipe->bufs[idx].ops,
			pipe->bufs[idx].page,
			pipe->bufs[idx].offset,
			pipe->bufs[idx].len);
	WARN_ON(1);
	return false;
}