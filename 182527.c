CWD_API int virtual_rename(char *oldname, char *newname TSRMLS_DC) /* {{{ */
{
	cwd_state old_state;
	cwd_state new_state;
	int retval;

	CWD_STATE_COPY(&old_state, &CWDG(cwd));
	if (virtual_file_ex(&old_state, oldname, NULL, CWD_EXPAND TSRMLS_CC)) {
		CWD_STATE_FREE(&old_state);
		return -1;
	}
	oldname = old_state.cwd;

	CWD_STATE_COPY(&new_state, &CWDG(cwd));
	if (virtual_file_ex(&new_state, newname, NULL, CWD_EXPAND TSRMLS_CC)) {
		CWD_STATE_FREE(&old_state);
		CWD_STATE_FREE(&new_state);
		return -1;
	}
	newname = new_state.cwd;

	/* rename on windows will fail if newname already exists.
	   MoveFileEx has to be used */
#ifdef TSRM_WIN32
	/* MoveFileEx returns 0 on failure, other way 'round for this function */
	retval = (MoveFileEx(oldname, newname, MOVEFILE_REPLACE_EXISTING|MOVEFILE_COPY_ALLOWED) == 0) ? -1 : 0;
#else
	retval = rename(oldname, newname);
#endif

	CWD_STATE_FREE(&old_state);
	CWD_STATE_FREE(&new_state);

	return retval;
}