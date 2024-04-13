assemble_mountinfo(struct parsed_mount_info *parsed_info,
		   const char *thisprogram, const char *mountpoint,
		   const char *orig_dev, char *orgoptions)
{
	int rc;
	char *newopts = NULL;

	rc = drop_capabilities(0);
	if (rc)
		goto assemble_exit;

	rc = drop_child_privs();
	if (rc)
		goto assemble_exit;

	if (getuid()) {
		rc = check_fstab(thisprogram, mountpoint, orig_dev,
				 &newopts);
		if (rc)
			goto assemble_exit;

		orgoptions = newopts;
		/* enable any default user mount flags */
		parsed_info->flags |= CIFS_SETUID_FLAGS;
	}

	rc = get_pw_from_env(parsed_info, thisprogram);
	if (rc)
		goto assemble_exit;

	if (orgoptions) {
		rc = parse_options(orgoptions, parsed_info);
		if (rc)
			goto assemble_exit;
	}

	if (getuid()) {
		if (!(parsed_info->flags & (MS_USERS | MS_USER))) {
			fprintf(stderr, "%s: permission denied\n", thisprogram);
			rc = EX_USAGE;
			goto assemble_exit;
		}
	}

	parsed_info->flags &= ~(MS_USERS | MS_USER);

	rc = parse_unc(orig_dev, parsed_info, thisprogram);
	if (rc)
		goto assemble_exit;

	if (parsed_info->addrlist[0] == '\0')
		rc = resolve_host(parsed_info->host, parsed_info->addrlist);

	switch (rc) {
	case EX_USAGE:
		fprintf(stderr, "mount error: could not resolve address for "
			"%s: %s\n", parsed_info->host,
			rc == EAI_SYSTEM ? strerror(errno) : gai_strerror(rc));
		goto assemble_exit;

	case EX_SYSERR:
		fprintf(stderr, "mount error: problem parsing address "
			"list: %s\n", strerror(errno));
		goto assemble_exit;
	}

	if (!parsed_info->got_user) {
		/*
		 * Note that the password will not be retrieved from the
		 * USER env variable (ie user%password form) as there is
		 * already a PASSWD environment varaible
		 */
		if (getenv("USER"))
			strlcpy(parsed_info->username, getenv("USER"),
				sizeof(parsed_info->username));
		else
			strlcpy(parsed_info->username, getusername(getuid()),
				sizeof(parsed_info->username));
		parsed_info->got_user = 1;
	}

	if (!parsed_info->got_password) {
		char tmp_pass[MOUNT_PASSWD_SIZE + 1];
		char *prompt = NULL;

		if(asprintf(&prompt, "Password for %s@%s: ", parsed_info->username, orig_dev) < 0)
			prompt = NULL;

		if (get_password(prompt ? prompt : "Password: ", tmp_pass, MOUNT_PASSWD_SIZE + 1)) {
			rc = set_password(parsed_info, tmp_pass);
		} else {
			fprintf(stderr, "Error reading password, exiting\n");
			rc = EX_SYSERR;
		}

		free(prompt);
		if (rc)
			goto assemble_exit;
	}

	/* copy in user= string */
	if (parsed_info->got_user) {
		if (*parsed_info->options)
			strlcat(parsed_info->options, ",",
				sizeof(parsed_info->options));
		strlcat(parsed_info->options, "user=",
			sizeof(parsed_info->options));
		strlcat(parsed_info->options, parsed_info->username,
			sizeof(parsed_info->options));
	}

	if (*parsed_info->domain) {
		if (*parsed_info->options)
			strlcat(parsed_info->options, ",",
				sizeof(parsed_info->options));
		strlcat(parsed_info->options, "domain=",
			sizeof(parsed_info->options));
		strlcat(parsed_info->options, parsed_info->domain,
			sizeof(parsed_info->options));
	} else if (parsed_info->got_domain) {
		strlcat(parsed_info->options, ",domain=",
			sizeof(parsed_info->options));
	}

assemble_exit:
	free(newopts);
	return rc;
}