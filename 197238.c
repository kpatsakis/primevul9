parse_opt(char *opt, int *mask, int *inv_user, char **extra_opts) {
	const struct opt_map *om;

	for (om = opt_map; om->opt != NULL; om++)
		if (streq (opt, om->opt)) {
			if (om->inv)
				*mask &= ~om->mask;
			else
				*mask |= om->mask;
			if (om->inv && ((*mask & MS_USER) || (*mask & MS_USERS))
			    && (om->mask & MS_SECURE))
				*inv_user |= om->mask;
			if ((om->mask == MS_USER || om->mask == MS_USERS)
			    && !om->inv)
				*mask |= MS_SECURE;
			if ((om->mask == MS_OWNER || om->mask == MS_GROUP)
			    && !om->inv)
				*mask |= MS_OWNERSECURE;
#ifdef MS_SILENT
			if (om->mask == MS_SILENT && om->inv)  {
				mount_quiet = 1;
				verbose = 0;
			}
#endif
			if (om->mask == MS_COMMENT) {
				comment_flags |= om->cmask;
				if (om->cmask == MS_COMMENT_NOFAIL)
					opt_nofail = 1;
			}
			return;
		}

	/* convert nonnumeric ids to numeric */
	if (!strncmp(opt, "uid=", 4) && !isdigit(opt[4])) {
		struct passwd *pw = getpwnam(opt+4);

		if (pw) {
			*extra_opts = append_numopt(*extra_opts,
						"uid=", pw->pw_uid);
			return;
		}
	}
	if (!strncmp(opt, "gid=", 4) && !isdigit(opt[4])) {
		struct group *gr = getgrnam(opt+4);

		if (gr) {
			*extra_opts = append_numopt(*extra_opts,
						"gid=", gr->gr_gid);
			return;
		}
	}

#ifdef HAVE_LIBSELINUX
	if (strncmp(opt, "context=", 8) == 0 && *(opt+8)) {
		if (append_context("context=", opt+8, extra_opts) == 0)
			return;
	}
	if (strncmp(opt, "fscontext=", 10) == 0 && *(opt+10)) {
		if (append_context("fscontext=", opt+10, extra_opts) == 0)
			return;
	}
	if (strncmp(opt, "defcontext=", 11) == 0 && *(opt+11)) {
		if (append_context("defcontext=", opt+11, extra_opts) == 0)
			return;
	}
	if (strncmp(opt, "rootcontext=", 12) == 0 && *(opt+12)) {
		if (append_context("rootcontext=", opt+12, extra_opts) == 0)
			return;
	}
#endif
	*extra_opts = append_opt(*extra_opts, opt, NULL);
}