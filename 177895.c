static int ps_files_key_exists(ps_files *data, const char *key TSRMLS_DC)
{
	char buf[MAXPATHLEN];
	struct stat sbuf;

	if (!key || !ps_files_path_create(buf, sizeof(buf), data, key)) {
		return FAILURE;
	}
	if (VCWD_STAT(buf, &sbuf)) {
		return FAILURE;
	}
	return SUCCESS;
}