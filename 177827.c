    get_nextpath(char *path, size_t * offsetp, size_t fulllen)
{
	size_t offset = *offsetp;

	if (offset >= fulllen)
		return NULL;

	while (offset < fulllen && path[offset] != '\0')
		offset++;
	while (offset < fulllen && path[offset] == '\0')
		offset++;

	*offsetp = offset;
	return (offset < fulllen) ? &path[offset] : NULL;
}