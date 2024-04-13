static void pipe_advance(struct iov_iter *i, size_t size)
{
	struct pipe_inode_info *pipe = i->pipe;
	if (size) {
		struct pipe_buffer *buf;
		unsigned int p_mask = pipe->ring_size - 1;
		unsigned int i_head = i->head;
		size_t off = i->iov_offset, left = size;

		if (off) /* make it relative to the beginning of buffer */
			left += off - pipe->bufs[i_head & p_mask].offset;
		while (1) {
			buf = &pipe->bufs[i_head & p_mask];
			if (left <= buf->len)
				break;
			left -= buf->len;
			i_head++;
		}
		i->head = i_head;
		i->iov_offset = buf->offset + left;
	}
	i->count -= size;
	/* ... and discard everything past that point */
	pipe_truncate(i);
}