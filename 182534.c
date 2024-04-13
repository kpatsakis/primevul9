CWD_API void virtual_cwd_startup(void) /* {{{ */
{
	char cwd[MAXPATHLEN];
	char *result;

#ifdef NETWARE
	result = getcwdpath(cwd, NULL, 1);
	if(result)
	{
		char *c=cwd;
		while(c = strchr(c, '\\'))
		{
			*c='/';
			++c;
		}
	}
#else
	result = getcwd(cwd, sizeof(cwd));
#endif
	if (!result) {
		cwd[0] = '\0';
	}

	main_cwd_state.cwd_length = strlen(cwd);
#ifdef TSRM_WIN32
	if (main_cwd_state.cwd_length >= 2 && cwd[1] == ':') {
		cwd[0] = toupper(cwd[0]);
	}
#endif
	main_cwd_state.cwd = strdup(cwd);

#ifdef ZTS
	ts_allocate_id(&cwd_globals_id, sizeof(virtual_cwd_globals), (ts_allocate_ctor) cwd_globals_ctor, (ts_allocate_dtor) cwd_globals_dtor);
#else
	cwd_globals_ctor(&cwd_globals TSRMLS_CC);
#endif

#if (defined(TSRM_WIN32) || defined(NETWARE)) && defined(ZTS)
	cwd_mutex = tsrm_mutex_alloc();
#endif
}