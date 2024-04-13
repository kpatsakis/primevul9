static const char *getpassf(const char *filename)
{
	STRUCT_STAT st;
	char buffer[512], *p;
	int n;

	if (!filename)
		return NULL;

	if (strcmp(filename, "-") == 0) {
		n = fgets(buffer, sizeof buffer, stdin) == NULL ? -1 : (int)strlen(buffer);
	} else {
		int fd;

		if ((fd = open(filename,O_RDONLY)) < 0) {
			rsyserr(FERROR, errno, "could not open password file %s", filename);
			exit_cleanup(RERR_SYNTAX);
		}

		if (do_stat(filename, &st) == -1) {
			rsyserr(FERROR, errno, "stat(%s)", filename);
			exit_cleanup(RERR_SYNTAX);
		}
		if ((st.st_mode & 06) != 0) {
			rprintf(FERROR, "ERROR: password file must not be other-accessible\n");
			exit_cleanup(RERR_SYNTAX);
		}
		if (MY_UID() == 0 && st.st_uid != 0) {
			rprintf(FERROR, "ERROR: password file must be owned by root when running as root\n");
			exit_cleanup(RERR_SYNTAX);
		}

		n = read(fd, buffer, sizeof buffer - 1);
		close(fd);
	}

	if (n > 0) {
		buffer[n] = '\0';
		if ((p = strtok(buffer, "\n\r")) != NULL)
			return strdup(p);
	}

	rprintf(FERROR, "ERROR: failed to read a password from %s\n", filename);
	exit_cleanup(RERR_SYNTAX);
}