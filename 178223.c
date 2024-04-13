const char **get_pathspec(const char *prefix, const char **pathspec)
{
	const char *entry = *pathspec;
	const char **src, **dst;
	int prefixlen;

	if (!prefix && !entry)
		return NULL;

	if (!entry) {
		static const char *spec[2];
		spec[0] = prefix;
		spec[1] = NULL;
		return spec;
	}

	/* Otherwise we have to re-write the entries.. */
	src = pathspec;
	dst = pathspec;
	prefixlen = prefix ? strlen(prefix) : 0;
	while (*src) {
		const char *p = prefix_path(prefix, prefixlen, *src);
		*(dst++) = p;
		src++;
	}
	*dst = NULL;
	if (!*pathspec)
		return NULL;
	return pathspec;
}