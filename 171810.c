static bool safe_append_string(char *dest,
			       const char *src,
			       int dest_buffer_size)
{
	int dest_length = strlen(dest);
	int src_length = strlen(src);

	if (dest_length + src_length + 1 > dest_buffer_size) {
		return false;
	}

	memcpy(dest + dest_length, src, src_length + 1);
	return true;
}