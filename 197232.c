fix_opts_string (int flags, const char *extra_opts,
		 const char *user, int inv_user)
{
	const struct opt_map *om;
	const struct string_opt_map *m;
	char *new_opts;

	new_opts = append_opt(NULL, (flags & MS_RDONLY) ? "ro" : "rw", NULL);
	for (om = opt_map; om->opt != NULL; om++) {
		if (om->skip)
			continue;
		if (om->inv || !om->mask || (flags & om->mask) != om->mask)
			continue;
		if (om->mask == MS_COMMENT && !(comment_flags & om->cmask))
			continue;
		new_opts = append_opt(new_opts, om->opt, NULL);
		flags &= ~om->mask;
	}
	for (m = &string_opt_map[0]; m->tag; m++) {
		if (!m->skip && *(m->valptr))
			new_opts = append_opt(new_opts, m->tag, *(m->valptr));
	}
	if (extra_opts && *extra_opts)
		new_opts = append_opt(new_opts, extra_opts, NULL);

	if (user)
		new_opts = append_opt(new_opts, "user=", user);

	if (inv_user) {
		for (om = opt_map; om->opt != NULL; om++) {
			if (om->mask && om->inv
			    && (inv_user & om->mask) == om->mask) {
				new_opts = append_opt(new_opts, om->opt, NULL);
				inv_user &= ~om->mask;
			}
		}
	}

	return new_opts;
}