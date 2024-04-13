static inline bool allocated(struct pipe_buffer *buf)
{
	return buf->ops == &default_pipe_buf_ops;
}