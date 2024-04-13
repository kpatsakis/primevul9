static enum ssh_variant determine_ssh_variant(const char *ssh_command,
					      int is_cmdline)
{
	enum ssh_variant ssh_variant = VARIANT_AUTO;
	const char *variant;
	char *p = NULL;

	override_ssh_variant(&ssh_variant);

	if (ssh_variant != VARIANT_AUTO)
		return ssh_variant;

	if (!is_cmdline) {
		p = xstrdup(ssh_command);
		variant = basename(p);
	} else {
		const char **ssh_argv;

		p = xstrdup(ssh_command);
		if (split_cmdline(p, &ssh_argv) > 0) {
			variant = basename((char *)ssh_argv[0]);
			/*
			 * At this point, variant points into the buffer
			 * referenced by p, hence we do not need ssh_argv
			 * any longer.
			 */
			free(ssh_argv);
		} else {
			free(p);
			return ssh_variant;
		}
	}

	if (!strcasecmp(variant, "ssh") ||
	    !strcasecmp(variant, "ssh.exe"))
		ssh_variant = VARIANT_SSH;
	else if (!strcasecmp(variant, "plink") ||
		 !strcasecmp(variant, "plink.exe"))
		ssh_variant = VARIANT_PLINK;
	else if (!strcasecmp(variant, "tortoiseplink") ||
		 !strcasecmp(variant, "tortoiseplink.exe"))
		ssh_variant = VARIANT_TORTOISEPLINK;

	free(p);
	return ssh_variant;
}