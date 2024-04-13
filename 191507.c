int commonio_open (struct commonio_db *db, int mode)
{
	char *buf;
	char *cp;
	char *line;
	struct commonio_entry *p;
	void *eptr = NULL;
	int flags = mode;
	size_t buflen;
	int fd;
	int saved_errno;

	mode &= ~O_CREAT;

	if (   db->isopen
	    || (   (O_RDONLY != mode)
	        && (O_RDWR != mode))) {
		errno = EINVAL;
		return 0;
	}
	db->readonly = (mode == O_RDONLY);
	if (!db->readonly && !db->locked) {
		errno = EACCES;
		return 0;
	}

	db->head = NULL;
	db->tail = NULL;
	db->cursor = NULL;
	db->changed = false;

	fd = open (db->filename,
	             (db->readonly ? O_RDONLY : O_RDWR)
	           | O_NOCTTY | O_NONBLOCK | O_NOFOLLOW);
	saved_errno = errno;
	db->fp = NULL;
	if (fd >= 0) {
#ifdef WITH_TCB
		if (tcb_is_suspect (fd) != 0) {
			(void) close (fd);
			errno = EINVAL;
			return 0;
		}
#endif				/* WITH_TCB */
		db->fp = fdopen (fd, db->readonly ? "r" : "r+");
		saved_errno = errno;
		if (NULL == db->fp) {
			(void) close (fd);
		}
	}
	errno = saved_errno;

	/*
	 * If O_CREAT was specified and the file didn't exist, it will be
	 * created by commonio_close().  We have no entries to read yet.  --marekm
	 */
	if (NULL == db->fp) {
		if (((flags & O_CREAT) != 0) && (ENOENT == errno)) {
			db->isopen = true;
			return 1;
		}
		return 0;
	}

	/* Do not inherit fd in spawned processes (e.g. nscd) */
	fcntl (fileno (db->fp), F_SETFD, FD_CLOEXEC);

	buflen = BUFLEN;
	buf = (char *) malloc (buflen);
	if (NULL == buf) {
		goto cleanup_ENOMEM;
	}

	while (db->ops->fgets (buf, (int) buflen, db->fp) == buf) {
		while (   ((cp = strrchr (buf, '\n')) == NULL)
		       && (feof (db->fp) == 0)) {
			size_t len;

			buflen += BUFLEN;
			cp = (char *) realloc (buf, buflen);
			if (NULL == cp) {
				goto cleanup_buf;
			}
			buf = cp;
			len = strlen (buf);
			if (db->ops->fgets (buf + len,
			                    (int) (buflen - len),
			                    db->fp) == NULL) {
				goto cleanup_buf;
			}
		}
		cp = strrchr (buf, '\n');
		if (NULL != cp) {
			*cp = '\0';
		}

		line = strdup (buf);
		if (NULL == line) {
			goto cleanup_buf;
		}

		if (name_is_nis (line)) {
			eptr = NULL;
		} else {
			eptr = db->ops->parse (line);
			if (NULL != eptr) {
				eptr = db->ops->dup (eptr);
				if (NULL == eptr) {
					goto cleanup_line;
				}
			}
		}

		p = (struct commonio_entry *) malloc (sizeof *p);
		if (NULL == p) {
			goto cleanup_entry;
		}

		p->eptr = eptr;
		p->line = line;
		p->changed = false;

		add_one_entry (db, p);
	}

	free (buf);

	if (ferror (db->fp) != 0) {
		goto cleanup_errno;
	}

	if ((NULL != db->ops->open_hook) && (db->ops->open_hook () == 0)) {
		goto cleanup_errno;
	}

	db->isopen = true;
	return 1;

      cleanup_entry:
	if (NULL != eptr) {
		db->ops->free (eptr);
	}
      cleanup_line:
	free (line);
      cleanup_buf:
	free (buf);
      cleanup_ENOMEM:
	errno = ENOMEM;
      cleanup_errno:
	saved_errno = errno;
	free_linked_list (db);
	fclose (db->fp);
	db->fp = NULL;
	errno = saved_errno;
	return 0;
}