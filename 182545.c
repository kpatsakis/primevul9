CWD_API FILE *virtual_popen(const char *command, const char *type TSRMLS_DC) /* {{{ */
{
	char prev_cwd[MAXPATHLEN];
	char *getcwd_result;
	FILE *retval;

	getcwd_result = VCWD_GETCWD(prev_cwd, MAXPATHLEN);
	if (!getcwd_result) {
		return NULL;
	}

#ifdef ZTS
	tsrm_mutex_lock(cwd_mutex);
#endif

	VCWD_CHDIR(CWDG(cwd).cwd);
	retval = popen(command, type);
	VCWD_CHDIR(prev_cwd);

#ifdef ZTS
	tsrm_mutex_unlock(cwd_mutex);
#endif

	return retval;
}