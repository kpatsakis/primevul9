static php_stream *_php_stream_fopen_from_fd_int(int fd, const char *mode, const char *persistent_id STREAMS_DC)
{
	php_stdio_stream_data *self;

	self = pemalloc_rel_orig(sizeof(*self), persistent_id);
	memset(self, 0, sizeof(*self));
	self->file = NULL;
	self->is_pipe = 0;
	self->lock_flag = LOCK_UN;
	self->is_process_pipe = 0;
	self->temp_name = NULL;
	self->fd = fd;
#ifdef PHP_WIN32
	self->is_pipe_blocking = 0;
#endif

	return php_stream_alloc_rel(&php_stream_stdio_ops, self, persistent_id, mode);
}