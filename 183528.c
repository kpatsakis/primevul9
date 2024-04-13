static bool safe_append_string(char *dest,
			       const char *src,
			       int dest_buffer_size)
{
	size_t len;
	len = strlcat(dest, src, dest_buffer_size);
	return (len < dest_buffer_size);
}