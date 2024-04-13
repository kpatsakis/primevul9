int lrename (const char *old, const char *new)
{
	int res;
	char *r = NULL;

#if defined(S_ISLNK)
#ifndef __GLIBC__
	char resolved_path[PATH_MAX];
#endif				/* !__GLIBC__ */
	struct stat sb;
	if (lstat (new, &sb) == 0 && S_ISLNK (sb.st_mode)) {
#ifdef __GLIBC__ /* now a POSIX.1-2008 feature */
		r = realpath (new, NULL);
#else				/* !__GLIBC__ */
		r = realpath (new, resolved_path);
#endif				/* !__GLIBC__ */
		if (NULL == r) {
			perror ("realpath in lrename()");
		} else {
			new = r;
		}
	}
#endif				/* S_ISLNK */

	res = rename (old, new);

#ifdef __GLIBC__
	if (NULL != r) {
		free (r);
	}
#endif				/* __GLIBC__ */

	return res;
}