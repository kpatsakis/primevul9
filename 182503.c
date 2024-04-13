CWD_API int virtual_chown(const char *filename, uid_t owner, gid_t group, int link TSRMLS_DC) /* {{{ */
{
	cwd_state new_state;
	int ret;

	CWD_STATE_COPY(&new_state, &CWDG(cwd));
	if (virtual_file_ex(&new_state, filename, NULL, CWD_REALPATH TSRMLS_CC)) {
		CWD_STATE_FREE(&new_state);
		return -1;
	}

	if (link) {
#if HAVE_LCHOWN
		ret = lchown(new_state.cwd, owner, group);
#else
		ret = -1;
#endif
	} else {
		ret = chown(new_state.cwd, owner, group);
	}

	CWD_STATE_FREE(&new_state);
	return ret;
}