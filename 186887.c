magic_stream(struct magic_set *ms, php_stream *stream)
{
	if (ms == NULL)
		return NULL;
	return file_or_stream(ms, NULL, stream);
}