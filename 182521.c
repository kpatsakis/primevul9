CWD_API realpath_cache_bucket* realpath_cache_lookup(const char *path, int path_len, time_t t TSRMLS_DC) /* {{{ */
{
	return realpath_cache_find(path, path_len, t TSRMLS_CC);
}