static void cwd_globals_ctor(virtual_cwd_globals *cwd_g TSRMLS_DC) /* {{{ */
{
	CWD_STATE_COPY(&cwd_g->cwd, &main_cwd_state);
	cwd_g->realpath_cache_size = 0;
	cwd_g->realpath_cache_size_limit = REALPATH_CACHE_SIZE;
	cwd_g->realpath_cache_ttl = REALPATH_CACHE_TTL;
	memset(cwd_g->realpath_cache, 0, sizeof(cwd_g->realpath_cache));
}