uncompressbuf(struct magic_set *ms, int fd, size_t method,
    const unsigned char *old, unsigned char **newch, size_t n)
{
	int fdin[2], fdout[2];
	ssize_t r;
	pid_t pid;

#ifdef BUILTIN_DECOMPRESS
        /* FIXME: This doesn't cope with bzip2 */
	if (method == 2)
		return uncompressgzipped(ms, old, newch, n);
#endif
	(void)fflush(stdout);
	(void)fflush(stderr);

	if ((fd != -1 && pipe(fdin) == -1) || pipe(fdout) == -1) {
		file_error(ms, errno, "cannot create pipe");	
		return NODATA;
	}
	switch (pid = fork()) {
	case 0:	/* child */
		(void) close(0);
		if (fd != -1) {
		    (void) dup(fd);
		    (void) FINFO_LSEEK_FUNC(0, (off_t)0, SEEK_SET);
		} else {
		    (void) dup(fdin[0]);
		    (void) close(fdin[0]);
		    (void) close(fdin[1]);
		}

		(void) close(1);
		(void) dup(fdout[1]);
		(void) close(fdout[0]);
		(void) close(fdout[1]);
#ifndef DEBUG
		if (compr[method].silent)
			(void)close(2);
#endif

		(void)execvp(compr[method].argv[0],
		    (char *const *)(intptr_t)compr[method].argv);
#ifdef DEBUG
		(void)fprintf(stderr, "exec `%s' failed (%s)\n",
		    compr[method].argv[0], strerror(errno));
#endif
		exit(1);
		/*NOTREACHED*/
	case -1:
		file_error(ms, errno, "could not fork");
		return NODATA;

	default: /* parent */
		(void) close(fdout[1]);
		if (fd == -1) {
			(void) close(fdin[0]);
			/* 
			 * fork again, to avoid blocking because both
			 * pipes filled
			 */
			switch (fork()) {
			case 0: /* child */
				(void)close(fdout[0]);
				if (swrite(fdin[1], old, n) != (ssize_t)n) {
#ifdef DEBUG
					(void)fprintf(stderr,
					    "Write failed (%s)\n",
					    strerror(errno));
#endif
					exit(1);
				}
				exit(0);
				/*NOTREACHED*/

			case -1:
#ifdef DEBUG
				(void)fprintf(stderr, "Fork failed (%s)\n",
				    strerror(errno));
#endif
				exit(1);
				/*NOTREACHED*/

			default:  /* parent */
				break;
			}
			(void) close(fdin[1]);
			fdin[1] = -1;
		}

		*newch = (unsigned char *) emalloc(HOWMANY + 1);

		if ((r = sread(fdout[0], *newch, HOWMANY, 0)) <= 0) {
#ifdef DEBUG
			(void)fprintf(stderr, "Read failed (%s)\n",
			    strerror(errno));
#endif
			efree(*newch);
			n = 0;
			newch[0] = '\0';
			goto err;
		} else {
			n = r;
		}
 		/* NUL terminate, as every buffer is handled here. */
 		(*newch)[n] = '\0';
err:
		if (fdin[1] != -1)
			(void) close(fdin[1]);
		(void) close(fdout[0]);
#ifdef WNOHANG
		while (waitpid(pid, NULL, WNOHANG) != -1)
			continue;
#else
		(void)wait(NULL);
#endif
		(void) close(fdin[0]);
	    
		return n;
	}
}