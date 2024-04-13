close_and_restore(const struct magic_set *ms, const char *name, int fd,
    const struct stat *sb)
{

	if ((ms->flags & MAGIC_PRESERVE_ATIME) != 0) {
		/*
		 * Try to restore access, modification times if read it.
		 * This is really *bad* because it will modify the status
		 * time of the file... And of course this will affect
		 * backup programs
		 */
#ifdef HAVE_UTIMES
		struct timeval  utsbuf[2];
		(void)memset(utsbuf, 0, sizeof(utsbuf));
		utsbuf[0].tv_sec = sb->st_atime;
		utsbuf[1].tv_sec = sb->st_mtime;

		(void) utimes(name, utsbuf); /* don't care if loses */
#elif defined(HAVE_UTIME_H) || defined(HAVE_SYS_UTIME_H)
		struct utimbuf  utbuf;

		(void)memset(&utbuf, 0, sizeof(utbuf));
		utbuf.actime = sb->st_atime;
		utbuf.modtime = sb->st_mtime;
		(void) utime(name, &utbuf); /* don't care if loses */
#endif
	}
}