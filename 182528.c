CWD_API char *virtual_getcwd(char *buf, size_t size TSRMLS_DC) /* {{{ */
{
	size_t length;
	char *cwd;

	cwd = virtual_getcwd_ex(&length TSRMLS_CC);

	if (buf == NULL) {
		return cwd;
	}
	if (length > size-1) {
		free(cwd);
		errno = ERANGE; /* Is this OK? */
		return NULL;
	}
	memcpy(buf, cwd, length+1);
	free(cwd);
	return buf;
}