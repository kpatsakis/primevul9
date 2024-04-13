PHPAPI php_stream *_php_stream_fopen_from_pipe(FILE *file, const char *mode STREAMS_DC)
{
	php_stdio_stream_data *self;
	php_stream *stream;

	self = emalloc_rel_orig(sizeof(*self));
	memset(self, 0, sizeof(*self));
	self->file = file;
	self->is_pipe = 1;
	self->lock_flag = LOCK_UN;
	self->is_process_pipe = 1;
	self->fd = fileno(file);
	self->temp_name = NULL;
#ifdef PHP_WIN32
	self->is_pipe_blocking = 0;
#endif

	stream = php_stream_alloc_rel(&php_stream_stdio_ops, self, 0, mode);
	stream->flags |= PHP_STREAM_FLAG_NO_SEEK;
	return stream;
}