static int php_stdiop_stat(php_stream *stream, php_stream_statbuf *ssb)
{
	int ret;
	php_stdio_stream_data *data = (php_stdio_stream_data*) stream->abstract;

	assert(data != NULL);
	if((ret = do_fstat(data, 1)) == 0) {
		memcpy(&ssb->sb, &data->sb, sizeof(ssb->sb));
	}

	return ret;
}