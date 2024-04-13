CWD_API void realpath_cache_clean(TSRMLS_D) /* {{{ */
{
	int i;

	for (i = 0; i < sizeof(CWDG(realpath_cache))/sizeof(CWDG(realpath_cache)[0]); i++) {
		realpath_cache_bucket *p = CWDG(realpath_cache)[i];
		while (p != NULL) {
			realpath_cache_bucket *r = p;
			p = p->next;
			free(r);
		}
		CWDG(realpath_cache)[i] = NULL;
	}
	CWDG(realpath_cache_size) = 0;
}