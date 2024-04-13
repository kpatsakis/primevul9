CWD_API FILE *virtual_popen(const char *command, const char *type TSRMLS_DC) /* {{{ */
{
	return popen_ex(command, type, CWDG(cwd).cwd, NULL TSRMLS_CC);
}