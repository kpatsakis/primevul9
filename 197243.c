check_special_mountprog(const char *spec, const char *node, const char *type, int flags,
			char *extra_opts, int *status) {
	char search_path[] = FS_SEARCH_PATH;
	char *path, mountprog[150];
	struct stat statbuf;
	int res;

	if (!external_allowed)
		return 0;

	if (type == NULL || strcmp(type, "none") == 0)
		return 0;

	path = strtok(search_path, ":");
	while (path) {
		int type_opt = 0;

		res = snprintf(mountprog, sizeof(mountprog), "%s/mount.%s",
			       path, type);
		path = strtok(NULL, ":");
		if (res < 0 || (size_t) res >= sizeof(mountprog))
			continue;

		res = stat(mountprog, &statbuf);
		if (res == -1 && errno == ENOENT && strchr(type, '.')) {
			/* If type ends with ".subtype" try without it */
			*strrchr(mountprog, '.') = '\0';
			type_opt = 1;
			res = stat(mountprog, &statbuf);
		}
		if (res)
			continue;

		if (verbose)
			fflush(stdout);

		switch (fork()) {
		case 0: { /* child */
			char *oo, *mountargs[12];
			int i = 0;

			if (setgid(getgid()) < 0)
				die(EX_FAIL, _("mount: cannot set group id: %m"));

			if (setuid(getuid()) < 0)
				die(EX_FAIL, _("mount: cannot set user id: %m"));

			oo = fix_opts_string(flags, extra_opts, NULL, invuser_flags);
			mountargs[i++] = mountprog;			/* 1 */
			mountargs[i++] = (char *) spec;			/* 2 */
			mountargs[i++] = (char *) node;			/* 3 */
			if (sloppy && strncmp(type, "nfs", 3) == 0)
				mountargs[i++] = "-s";			/* 4 */
			if (fake)
				mountargs[i++] = "-f";			/* 5 */
			if (nomtab)
				mountargs[i++] = "-n";			/* 6 */
			if (verbose)
				mountargs[i++] = "-v";			/* 7 */
			if (oo && *oo) {
				mountargs[i++] = "-o";			/* 8 */
				mountargs[i++] = oo;			/* 9 */
			}
			if (type_opt) {
				mountargs[i++] = "-t";			/* 10 */
				mountargs[i++] = (char *) type;		/* 11 */
			}
			mountargs[i] = NULL;				/* 12 */

			if (verbose > 2) {
				i = 0;
				while (mountargs[i]) {
					printf("mount: external mount: argv[%d] = \"%s\"\n",
						i, mountargs[i]);
					i++;
				}
				fflush(stdout);
			}

			execv(mountprog, mountargs);
			exit(1);	/* exec failed */
		}

		default: { /* parent */
			int st;
			wait(&st);
			*status = (WIFEXITED(st) ? WEXITSTATUS(st) : EX_SYSERR);
			return 1;
		}

		case -1: { /* error */
			int errsv = errno;
			error(_("mount: cannot fork: %s"), strerror(errsv));
		}
		}
	}

	return 0;
}