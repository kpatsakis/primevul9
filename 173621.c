static int php_zip_ops_close(php_stream *stream, int close_handle TSRMLS_DC)
{
	STREAM_DATA_FROM_STREAM();
	if (close_handle) {
		if (self->zf) {
			zip_fclose(self->zf);
			self->zf = NULL;
		}

		if (self->za) {
			zip_close(self->za);
			self->za = NULL;
		}
	}
	efree(self);
	stream->abstract = NULL;
	return EOF;
}