static int php_is_file_ok(const cwd_state *state)  /* {{{ */
{
	struct stat buf;

	if (php_sys_stat(state->cwd, &buf) == 0 && S_ISREG(buf.st_mode))
		return (0);

	return (1);
}