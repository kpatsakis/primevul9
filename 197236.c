parse_opts (const char *options, int *flags, char **extra_opts) {
	*flags = 0;
	*extra_opts = NULL;

	clear_string_opts();
	clear_flags_opts();

	if (options != NULL) {
		char *opts = xstrdup(options);
		int open_quote = 0;
		char *opt, *p;

		for (p=opts, opt=NULL; p && *p; p++) {
			if (!opt)
				opt = p;		/* begin of the option item */
			if (*p == '"')
				open_quote ^= 1;	/* reverse the status */
			if (open_quote)
				continue;		/* still in quoted block */
			if (*p == ',')
				*p = '\0';		/* terminate the option item */
			/* end of option item or last item */
			if (*p == '\0' || *(p+1) == '\0') {
				if (!parse_string_opt(opt))
					parse_opt(opt, flags, &invuser_flags, extra_opts);
				opt = NULL;
			}
		}
		free(opts);
	}

	if (readonly)
		*flags |= MS_RDONLY;
	if (readwrite)
		*flags &= ~MS_RDONLY;

	*flags |= mounttype;

	/* The propagation flags should not be used together with any
	 * other flags (except MS_REC and MS_SILENT) */
	if (*flags & MS_PROPAGATION)
		*flags &= (MS_PROPAGATION | MS_REC | MS_SILENT);
}