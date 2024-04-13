CWD_API int virtual_filepath_ex(const char *path, char **filepath, verify_path_func verify_path TSRMLS_DC) /* {{{ */
{
	cwd_state new_state;
	int retval;

	CWD_STATE_COPY(&new_state, &CWDG(cwd));
	retval = virtual_file_ex(&new_state, path, verify_path, CWD_FILEPATH TSRMLS_CC);

	*filepath = new_state.cwd;

	return retval;

}