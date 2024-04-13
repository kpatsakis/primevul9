static inline unsigned long realpath_cache_key(const char *path, int path_len TSRMLS_DC) /* {{{ */
{
	register unsigned long h;
	char *bucket_key_start = tsrm_win32_get_path_sid_key(path TSRMLS_CC);
	char *bucket_key = (char *)bucket_key_start;
	const char *e = bucket_key + strlen(bucket_key);

	if (!bucket_key) {
		return 0;
	}

	for (h = 2166136261U; bucket_key < e;) {
		h *= 16777619;
		h ^= *bucket_key++;
	}
	HeapFree(GetProcessHeap(), 0, (LPVOID)bucket_key_start);
	return h;
}