static inline unsigned long realpath_cache_key(const char *path, int path_len) /* {{{ */
{
	register unsigned long h;
	const char *e = path + path_len;

	for (h = 2166136261U; path < e;) {
		h *= 16777619;
		h ^= *path++;
	}

	return h;
}