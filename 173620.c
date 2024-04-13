static size_t php_zip_ops_read(php_stream *stream, char *buf, size_t count TSRMLS_DC)
{
	ssize_t n = 0;
	STREAM_DATA_FROM_STREAM();

	if (self->za && self->zf) {
		n = zip_fread(self->zf, buf, count);
		if (n < 0) {
			int ze, se;
			zip_file_error_get(self->zf, &ze, &se);
			stream->eof = 1;
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Zip stream error: %s", zip_file_strerror(self->zf));
			return 0;
		}
		/* cast count to signed value to avoid possibly negative n
		 * being cast to unsigned value */
		if (n == 0 || n < (ssize_t)count) {
			stream->eof = 1;
		} else {
			self->cursor += n;
		}
	}
	return (n < 1 ? 0 : (size_t)n);
}