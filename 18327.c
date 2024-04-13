static inline void data_start(const struct iov_iter *i,
			      unsigned int *iter_headp, size_t *offp)
{
	unsigned int p_mask = i->pipe->ring_size - 1;
	unsigned int iter_head = i->head;
	size_t off = i->iov_offset;

	if (off && (!allocated(&i->pipe->bufs[iter_head & p_mask]) ||
		    off == PAGE_SIZE)) {
		iter_head++;
		off = 0;
	}
	*iter_headp = iter_head;
	*offp = off;
}