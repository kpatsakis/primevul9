static inline void pipe_truncate(struct iov_iter *i)
{
	struct pipe_inode_info *pipe = i->pipe;
	unsigned int p_tail = pipe->tail;
	unsigned int p_head = pipe->head;
	unsigned int p_mask = pipe->ring_size - 1;

	if (!pipe_empty(p_head, p_tail)) {
		struct pipe_buffer *buf;
		unsigned int i_head = i->head;
		size_t off = i->iov_offset;

		if (off) {
			buf = &pipe->bufs[i_head & p_mask];
			buf->len = off - buf->offset;
			i_head++;
		}
		while (p_head != i_head) {
			p_head--;
			pipe_buf_release(pipe, &pipe->bufs[p_head & p_mask]);
		}

		pipe->head = p_head;
	}
}