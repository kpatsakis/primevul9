void iov_iter_pipe(struct iov_iter *i, unsigned int direction,
			struct pipe_inode_info *pipe,
			size_t count)
{
	BUG_ON(direction != READ);
	WARN_ON(pipe_full(pipe->head, pipe->tail, pipe->ring_size));
	*i = (struct iov_iter){
		.iter_type = ITER_PIPE,
		.data_source = false,
		.pipe = pipe,
		.head = pipe->head,
		.start_head = pipe->head,
		.iov_offset = 0,
		.count = count
	};
}