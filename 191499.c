static int do_lock_file (const char *file, const char *lock, bool log)
{
	int fd;
	pid_t pid;
	ssize_t len;
	int retval;
	char buf[32];

	fd = open (file, O_CREAT | O_EXCL | O_WRONLY, 0600);
	if (-1 == fd) {
		if (log) {
			(void) fprintf (stderr,
			                "%s: %s: %s\n",
			                Prog, file, strerror (errno));
		}
		return 0;
	}

	pid = getpid ();
	snprintf (buf, sizeof buf, "%lu", (unsigned long) pid);
	len = (ssize_t) strlen (buf) + 1;
	if (write (fd, buf, (size_t) len) != len) {
		if (log) {
			(void) fprintf (stderr,
			                "%s: %s: %s\n",
			                Prog, file, strerror (errno));
		}
		(void) close (fd);
		unlink (file);
		return 0;
	}
	close (fd);

	if (link (file, lock) == 0) {
		retval = check_link_count (file);
		if ((0==retval) && log) {
			(void) fprintf (stderr,
			                "%s: %s: lock file already used\n",
			                Prog, file);
		}
		unlink (file);
		return retval;
	}

	fd = open (lock, O_RDWR);
	if (-1 == fd) {
		if (log) {
			(void) fprintf (stderr,
			                "%s: %s: %s\n",
			                Prog, lock, strerror (errno));
		}
		unlink (file);
		errno = EINVAL;
		return 0;
	}
	len = read (fd, buf, sizeof (buf) - 1);
	close (fd);
	if (len <= 0) {
		if (log) {
			(void) fprintf (stderr,
			                "%s: existing lock file %s without a PID\n",
			                Prog, lock);
		}
		unlink (file);
		errno = EINVAL;
		return 0;
	}
	buf[len] = '\0';
	if (get_pid (buf, &pid) == 0) {
		if (log) {
			(void) fprintf (stderr,
			                "%s: existing lock file %s with an invalid PID '%s'\n",
			                Prog, lock, buf);
		}
		unlink (file);
		errno = EINVAL;
		return 0;
	}
	if (kill (pid, 0) == 0) {
		if (log) {
			(void) fprintf (stderr,
			                "%s: lock %s already used by PID %lu\n",
			                Prog, lock, (unsigned long) pid);
		}
		unlink (file);
		errno = EEXIST;
		return 0;
	}
	if (unlink (lock) != 0) {
		if (log) {
			(void) fprintf (stderr,
			                "%s: cannot get lock %s: %s\n",
			                Prog, lock, strerror (errno));
		}
		unlink (file);
		return 0;
	}

	retval = 0;
	if (link (file, lock) == 0) {
		retval = check_link_count (file);
		if ((0==retval) && log) {
			(void) fprintf (stderr,
			                "%s: %s: lock file already used\n",
			                Prog, file);
		}
	} else {
		if (log) {
			(void) fprintf (stderr,
			                "%s: cannot get lock %s: %s\n",
			                Prog, lock, strerror (errno));
		}
	}

	unlink (file);
	return retval;
}