CWD_API void virtual_cwd_shutdown(void) /* {{{ */
{
#ifndef ZTS
	cwd_globals_dtor(&cwd_globals TSRMLS_CC);
#endif
#if (defined(TSRM_WIN32) || defined(NETWARE)) && defined(ZTS)
	tsrm_mutex_free(cwd_mutex);
#endif

	free(main_cwd_state.cwd); /* Don't use CWD_STATE_FREE because the non global states will probably use emalloc()/efree() */
}