int bpf_obj_name_cpy(char *dst, const char *src, unsigned int size)
{
	const char *end = src + size;
	const char *orig_src = src;

	memset(dst, 0, size);
	/* Copy all isalnum(), '_' and '.' chars. */
	while (src < end && *src) {
		if (!isalnum(*src) &&
		    *src != '_' && *src != '.')
			return -EINVAL;
		*dst++ = *src++;
	}

	/* No '\0' found in "size" number of bytes */
	if (src == end)
		return -EINVAL;

	return src - orig_src;
}