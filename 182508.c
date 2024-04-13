static inline void realpath_cache_add(const char *path, int path_len, const char *realpath, int realpath_len, int is_dir, time_t t TSRMLS_DC) /* {{{ */
{
	long size = sizeof(realpath_cache_bucket) + path_len + 1;
	int same = 1;

	if (realpath_len != path_len ||
		memcmp(path, realpath, path_len) != 0) {
		size += realpath_len + 1;
		same = 0;
	}

	if (CWDG(realpath_cache_size) + size <= CWDG(realpath_cache_size_limit)) {
		realpath_cache_bucket *bucket = malloc(size);
		unsigned long n;

		if (bucket == NULL) {
			return;
		}

#ifdef PHP_WIN32
		bucket->key = realpath_cache_key(path, path_len TSRMLS_CC);
#else
		bucket->key = realpath_cache_key(path, path_len);
#endif
		bucket->path = (char*)bucket + sizeof(realpath_cache_bucket);
		memcpy(bucket->path, path, path_len+1);
		bucket->path_len = path_len;
		if (same) {
			bucket->realpath = bucket->path;
		} else {
			bucket->realpath = bucket->path + (path_len + 1);
			memcpy(bucket->realpath, realpath, realpath_len+1);
		}
		bucket->realpath_len = realpath_len;
		bucket->is_dir = is_dir;
#ifdef PHP_WIN32
		bucket->is_rvalid   = 0;
		bucket->is_readable = 0;
		bucket->is_wvalid   = 0;
		bucket->is_writable = 0;
#endif
		bucket->expires = t + CWDG(realpath_cache_ttl);
		n = bucket->key % (sizeof(CWDG(realpath_cache)) / sizeof(CWDG(realpath_cache)[0]));
		bucket->next = CWDG(realpath_cache)[n];
		CWDG(realpath_cache)[n] = bucket;
		CWDG(realpath_cache_size) += size;
	}
}