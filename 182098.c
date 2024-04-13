static void push_ssh_options(struct argv_array *args, struct argv_array *env,
			     enum ssh_variant variant, const char *port,
			     enum protocol_version version, int flags)
{
	if (variant == VARIANT_SSH &&
	    version > 0) {
		argv_array_push(args, "-o");
		argv_array_push(args, "SendEnv=" GIT_PROTOCOL_ENVIRONMENT);
		argv_array_pushf(env, GIT_PROTOCOL_ENVIRONMENT "=version=%d",
				 version);
	}

	if (flags & CONNECT_IPV4) {
		switch (variant) {
		case VARIANT_AUTO:
			BUG("VARIANT_AUTO passed to push_ssh_options");
		case VARIANT_SIMPLE:
			die(_("ssh variant 'simple' does not support -4"));
		case VARIANT_SSH:
		case VARIANT_PLINK:
		case VARIANT_PUTTY:
		case VARIANT_TORTOISEPLINK:
			argv_array_push(args, "-4");
		}
	} else if (flags & CONNECT_IPV6) {
		switch (variant) {
		case VARIANT_AUTO:
			BUG("VARIANT_AUTO passed to push_ssh_options");
		case VARIANT_SIMPLE:
			die(_("ssh variant 'simple' does not support -6"));
		case VARIANT_SSH:
		case VARIANT_PLINK:
		case VARIANT_PUTTY:
		case VARIANT_TORTOISEPLINK:
			argv_array_push(args, "-6");
		}
	}

	if (variant == VARIANT_TORTOISEPLINK)
		argv_array_push(args, "-batch");

	if (port) {
		switch (variant) {
		case VARIANT_AUTO:
			BUG("VARIANT_AUTO passed to push_ssh_options");
		case VARIANT_SIMPLE:
			die(_("ssh variant 'simple' does not support setting port"));
		case VARIANT_SSH:
			argv_array_push(args, "-p");
			break;
		case VARIANT_PLINK:
		case VARIANT_PUTTY:
		case VARIANT_TORTOISEPLINK:
			argv_array_push(args, "-P");
		}

		argv_array_push(args, port);
	}
}