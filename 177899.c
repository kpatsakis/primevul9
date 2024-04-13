static int ps_files_cleanup_dir(const char *dirname, int maxlifetime TSRMLS_DC)
{
	DIR *dir;
	char dentry[sizeof(struct dirent) + MAXPATHLEN];
	struct dirent *entry = (struct dirent *) &dentry;
	struct stat sbuf;
	char buf[MAXPATHLEN];
	time_t now;
	int nrdels = 0;
	size_t dirname_len;

	dir = opendir(dirname);
	if (!dir) {
		php_error_docref(NULL TSRMLS_CC, E_NOTICE, "ps_files_cleanup_dir: opendir(%s) failed: %s (%d)", dirname, strerror(errno), errno);
		return (0);
	}

	time(&now);

	dirname_len = strlen(dirname);

	/* Prepare buffer (dirname never changes) */
	memcpy(buf, dirname, dirname_len);
	buf[dirname_len] = PHP_DIR_SEPARATOR;

	while (php_readdir_r(dir, (struct dirent *) dentry, &entry) == 0 && entry) {
		/* does the file start with our prefix? */
		if (!strncmp(entry->d_name, FILE_PREFIX, sizeof(FILE_PREFIX) - 1)) {
			size_t entry_len = strlen(entry->d_name);

			/* does it fit into our buffer? */
			if (entry_len + dirname_len + 2 < MAXPATHLEN) {
				/* create the full path.. */
				memcpy(buf + dirname_len + 1, entry->d_name, entry_len);

				/* NUL terminate it and */
				buf[dirname_len + entry_len + 1] = '\0';

				/* check whether its last access was more than maxlifetime ago */
				if (VCWD_STAT(buf, &sbuf) == 0 &&
						(now - sbuf.st_mtime) > maxlifetime) {
					VCWD_UNLINK(buf);
					nrdels++;
				}
			}
		}
	}

	closedir(dir);

	return (nrdels);
}