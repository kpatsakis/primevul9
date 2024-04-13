static /*@null@*/ /*@dependent@*/FILE *fopen_set_perms (
	const char *name,
	const char *mode,
	const struct stat *sb)
{
	FILE *fp;
	mode_t mask;

	mask = umask (0777);
	fp = fopen (name, mode);
	(void) umask (mask);
	if (NULL == fp) {
		return NULL;
	}

#ifdef HAVE_FCHOWN
	if (fchown (fileno (fp), sb->st_uid, sb->st_gid) != 0) {
		goto fail;
	}
#else				/* !HAVE_FCHOWN */
	if (chown (name, sb->st_mode) != 0) {
		goto fail;
	}
#endif				/* !HAVE_FCHOWN */

#ifdef HAVE_FCHMOD
	if (fchmod (fileno (fp), sb->st_mode & 0664) != 0) {
		goto fail;
	}
#else				/* !HAVE_FCHMOD */
	if (chmod (name, sb->st_mode & 0664) != 0) {
		goto fail;
	}
#endif				/* !HAVE_FCHMOD */
	return fp;

      fail:
	(void) fclose (fp);
	/* fopen_set_perms is used for intermediate files */
	(void) unlink (name);
	return NULL;
}