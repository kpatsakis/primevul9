size_t git_path_basename_offset(git_buf *buffer)
{
	ssize_t slash;

	if (!buffer || buffer->size <= 0)
		return 0;

	slash = git_buf_rfind_next(buffer, '/');

	if (slash >= 0 && buffer->ptr[slash] == '/')
		return (size_t)(slash + 1);

	return 0;
}