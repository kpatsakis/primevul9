static int create_backup (const char *backup, FILE * fp)
{
	struct stat sb;
	struct utimbuf ub;
	FILE *bkfp;
	int c;

	if (fstat (fileno (fp), &sb) != 0) {
		return -1;
	}

	bkfp = fopen_set_perms (backup, "w", &sb);
	if (NULL == bkfp) {
		return -1;
	}

	/* TODO: faster copy, not one-char-at-a-time.  --marekm */
	c = 0;
	if (fseek (fp, 0, SEEK_SET) == 0) {
		while ((c = getc (fp)) != EOF) {
			if (putc (c, bkfp) == EOF) {
				break;
			}
		}
	}
	if ((c != EOF) || (ferror (fp) != 0) || (fflush (bkfp) != 0)) {
		(void) fclose (bkfp);
		/* FIXME: unlink the backup file? */
		return -1;
	}
	if (   (fsync (fileno (bkfp)) != 0)
	    || (fclose (bkfp) != 0)) {
		/* FIXME: unlink the backup file? */
		return -1;
	}

	ub.actime = sb.st_atime;
	ub.modtime = sb.st_mtime;
	(void) utime (backup, &ub);
	return 0;
}