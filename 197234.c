do_mount_all (char *types, char *options, char *test_opts) {
	struct mntentchn *mc, *mc0, *mtmp;
	int status = 0;
	struct stat statbuf;
	struct child {
		pid_t pid;
		char *group;
		struct mntentchn *mec;
		struct mntentchn *meclast;
		struct child *nxt;
	} childhead, *childtail, *cp;
	char major[22];
	char *g, *colon;

	/* build a chain of what we have to do, or maybe
	   several chains, one for each major or NFS host */
	childhead.nxt = 0;
	childtail = &childhead;
	mc0 = fstab_head();
	for (mc = mc0->nxt; mc && mc != mc0; mc = mc->nxt) {
		if (has_noauto (mc->m.mnt_opts))
			continue;
		if (matching_type (mc->m.mnt_type, types)
		    && matching_opts (mc->m.mnt_opts, test_opts)
		    && !streq (mc->m.mnt_dir, "/")
		    && !streq (mc->m.mnt_dir, "root")
		    && !is_fstab_entry_mounted(mc, verbose)) {

			mtmp = (struct mntentchn *) xmalloc(sizeof(*mtmp));
			*mtmp = *mc;
			mtmp->nxt = 0;
			g = NULL;
			if (optfork) {
				if (stat(mc->m.mnt_fsname, &statbuf) == 0 &&
				    S_ISBLK(statbuf.st_mode)) {
					sprintf(major, "#%x",
						DISKMAJOR(statbuf.st_rdev));
					g = major;
				}
				if (strcmp(mc->m.mnt_type, "nfs") == 0) {
					g = xstrdup(mc->m.mnt_fsname);
					colon = strchr(g, ':');
					if (colon)
						*colon = '\0';
				}
			}
			if (g) {
				for (cp = childhead.nxt; cp; cp = cp->nxt)
					if (cp->group &&
					    strcmp(cp->group, g) == 0) {
						cp->meclast->nxt = mtmp;
						cp->meclast = mtmp;
						goto fnd;
					}
			}
			cp = (struct child *) xmalloc(sizeof *cp);
			cp->nxt = 0;
			cp->mec = cp->meclast = mtmp;
			cp->group = xstrdup(g);
			cp->pid = 0;
			childtail->nxt = cp;
			childtail = cp;
		fnd:;

		}
	}

	/* now do everything */
	for (cp = childhead.nxt; cp; cp = cp->nxt) {
		pid_t p = -1;
		if (optfork) {
			p = fork();
			if (p == -1) {
				int errsv = errno;
				error(_("mount: cannot fork: %s"),
				      strerror (errsv));
			}
			else if (p != 0)
				cp->pid = p;
		}

		/* if child, or not forked, do the mounting */
		if (p == 0 || p == -1) {
			for (mc = cp->mec; mc; mc = mc->nxt) {
				status |= mount_one (mc->m.mnt_fsname,
						     mc->m.mnt_dir,
						     mc->m.mnt_type,
						     mc->m.mnt_opts,
						     options, 0, 0);
			}
			if (mountcount)
				status |= EX_SOMEOK;
			if (p == 0)
				exit(status);
		}
	}

	/* wait for children, if any */
	while ((cp = childhead.nxt) != NULL) {
		childhead.nxt = cp->nxt;
		if (cp->pid) {
			int ret;
		keep_waiting:
			if(waitpid(cp->pid, &ret, 0) == -1) {
				if (errno == EINTR)
					goto keep_waiting;
				perror("waitpid");
			} else if (WIFEXITED(ret))
				status |= WEXITSTATUS(ret);
			else
				status |= EX_SYSERR;
		}
	}
	if (mountcount)
		status |= EX_SOMEOK;
	return status;
}