CWD_API int virtual_chdir_file(const char *path, int (*p_chdir)(const char *path TSRMLS_DC) TSRMLS_DC) /* {{{ */
{
	int length = strlen(path);
	char *temp;
	int retval;
	TSRM_ALLOCA_FLAG(use_heap)

	if (length == 0) {
		return 1; /* Can't cd to empty string */
	}
	while(--length >= 0 && !IS_SLASH(path[length])) {
	}

	if (length == -1) {
		/* No directory only file name */
		errno = ENOENT;
		return -1;
	}

	if (length == COPY_WHEN_ABSOLUTE(path) && IS_ABSOLUTE_PATH(path, length+1)) { /* Also use trailing slash if this is absolute */
		length++;
	}
	temp = (char *) tsrm_do_alloca(length+1, use_heap);
	memcpy(temp, path, length);
	temp[length] = 0;
#if VIRTUAL_CWD_DEBUG
	fprintf (stderr, "Changing directory to %s\n", temp);
#endif
	retval = p_chdir(temp TSRMLS_CC);
	tsrm_free_alloca(temp, use_heap);
	return retval;
}