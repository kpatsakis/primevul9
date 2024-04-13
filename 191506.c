int commonio_close (struct commonio_db *db)
	/*@requires notnull db->fp@*/
{
	char buf[1024];
	int errors = 0;
	struct stat sb;

	if (!db->isopen) {
		errno = EINVAL;
		return 0;
	}
	db->isopen = false;

	if (!db->changed || db->readonly) {
		(void) fclose (db->fp);
		db->fp = NULL;
		goto success;
	}

	if ((NULL != db->ops->close_hook) && (db->ops->close_hook () == 0)) {
		goto fail;
	}

	memzero (&sb, sizeof sb);
	if (NULL != db->fp) {
		if (fstat (fileno (db->fp), &sb) != 0) {
			(void) fclose (db->fp);
			db->fp = NULL;
			goto fail;
		}

		/*
		 * Create backup file.
		 */
		snprintf (buf, sizeof buf, "%s-", db->filename);

#ifdef WITH_SELINUX
		if (set_selinux_file_context (buf) != 0) {
			errors++;
		}
#endif
		if (create_backup (buf, db->fp) != 0) {
			errors++;
		}

		if (fclose (db->fp) != 0) {
			errors++;
		}

#ifdef WITH_SELINUX
		if (reset_selinux_file_context () != 0) {
			errors++;
		}
#endif
		if (errors != 0) {
			db->fp = NULL;
			goto fail;
		}
	} else {
		/*
		 * Default permissions for new [g]shadow files.
		 */
		sb.st_mode = db->st_mode;
		sb.st_uid = db->st_uid;
		sb.st_gid = db->st_gid;
	}

	snprintf (buf, sizeof buf, "%s+", db->filename);

#ifdef WITH_SELINUX
	if (set_selinux_file_context (buf) != 0) {
		errors++;
	}
#endif

	db->fp = fopen_set_perms (buf, "w", &sb);
	if (NULL == db->fp) {
		goto fail;
	}

	if (write_all (db) != 0) {
		errors++;
	}

	if (fflush (db->fp) != 0) {
		errors++;
	}
#ifdef HAVE_FSYNC
	if (fsync (fileno (db->fp)) != 0) {
		errors++;
	}
#else				/* !HAVE_FSYNC */
	sync ();
#endif				/* !HAVE_FSYNC */
	if (fclose (db->fp) != 0) {
		errors++;
	}

	db->fp = NULL;

	if (errors != 0) {
		unlink (buf);
		goto fail;
	}

	if (lrename (buf, db->filename) != 0) {
		goto fail;
	}

#ifdef WITH_SELINUX
	if (reset_selinux_file_context () != 0) {
		goto fail;
	}
#endif

	nscd_need_reload = true;
	goto success;
      fail:
	errors++;
      success:

	free_linked_list (db);
	return errors == 0;
}