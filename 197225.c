mount_one (const char *spec, const char *node, const char *types,
	   const char *fstabopts, char *cmdlineopts, int freq, int pass) {
	const char *nspec = NULL;
	char *opts;

	/* Substitute values in opts, if required */
	opts = usersubst(fstabopts);

	/* Merge the fstab and command line options.  */
	opts = append_opt(opts, cmdlineopts, NULL);

	if (types == NULL && !mounttype && !is_existing_file(spec)) {
		if (strchr (spec, ':') != NULL) {
			types = "nfs";
			if (verbose)
				printf(_("mount: no type was given - "
					 "I'll assume nfs because of "
					 "the colon\n"));
		} else if(!strncmp(spec, "//", 2)) {
			types = "cifs";
			if (verbose)
				printf(_("mount: no type was given - "
					 "I'll assume cifs because of "
					 "the // prefix\n"));
		}
	}

	/* Handle possible LABEL= and UUID= forms of spec */
	if (types == NULL || (strncmp(types, "9p", 2) &&
			      strncmp(types, "nfs", 3) &&
			      strncmp(types, "cifs", 4) &&
			      strncmp(types, "smbfs", 5))) {
		if (!is_pseudo_fs(types))
			nspec = spec_to_devname(spec);
		if (nspec)
			spec = nspec;
	}

	return try_mount_one (spec, node, types, opts, freq, pass, 0);
}