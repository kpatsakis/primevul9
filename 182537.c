CWD_API int realpath_cache_max_buckets(TSRMLS_D)
{
	return (sizeof(CWDG(realpath_cache)) / sizeof(CWDG(realpath_cache)[0]));
}