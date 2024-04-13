static php_stream *_php_stream_fopen_from_file_int(FILE *file, const char *mode STREAMS_DC)
{
	php_stdio_stream_data *self;

	self = emalloc_rel_orig(sizeof(*self));
	memset(self, 0, sizeof(*self));
	self->file = file;
	self->is_pipe = 0;
	self->lock_flag = LOCK_UN;
	self->is_process_pipe = 0;
	self->temp_name = NULL;
	self->fd = fileno(file);
#ifdef PHP_WIN32
	self->is_pipe_blocking = 0;
#endif

	return php_stream_alloc_rel(&php_stream_stdio_ops, self, 0, mode);
}