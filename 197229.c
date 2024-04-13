static char *remove_context_options(char *opts)
{
	char *begin = NULL, *end = NULL, *p;
	int open_quote = 0, changed = 0;

	if (!opts)
		return NULL;

	opts = xstrdup(opts);

	for (p = opts; p && *p; p++) {
		if (!begin)
			begin = p;		/* begin of the option item */
		if (*p == '"')
			open_quote ^= 1;	/* reverse the status */
		if (open_quote)
			continue;		/* still in quoted block */
		if (*p == ',')
			end = p;		/* terminate the option item */
		else if (*(p + 1) == '\0')
			end = p + 1;		/* end of optstr */
		if (!begin || !end)
			continue;

		if (strncmp(begin, "context=", 8) == 0 ||
		    strncmp(begin, "fscontext=", 10) == 0 ||
		    strncmp(begin, "defcontext=", 11) == 0 ||
	            strncmp(begin, "rootcontext=", 12) == 0 ||
		    strncmp(begin, "seclabel", 8) == 0) {
			size_t sz;

			if ((begin == opts || *(begin - 1) == ',') && *end == ',')
				end++;
			sz = strlen(end);

			memmove(begin, end, sz + 1);
			if (!*begin && *(begin - 1) == ',')
				*(begin - 1) = '\0';

			p = begin;
			changed = 1;
		}
		begin = end = NULL;
	}

	if (changed && verbose)
		printf (_("mount: SELinux *context= options are ignore on remount.\n"));

	return opts;
}