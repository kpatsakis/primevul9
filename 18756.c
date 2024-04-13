static void __attribute__((__noreturn__)) exit_non_root(const char *option)
{
	const uid_t ruid = getuid();
	const uid_t euid = geteuid();

	if (ruid == 0 && euid != 0) {
		/* user is root, but setuid to non-root */
		if (option)
			errx(MOUNT_EX_USAGE, _("only root can use \"--%s\" option "
					 "(effective UID is %u)"),
					option, euid);
		errx(MOUNT_EX_USAGE, _("only root can do that "
				 "(effective UID is %u)"), euid);
	}
	if (option)
		errx(MOUNT_EX_USAGE, _("only root can use \"--%s\" option"), option);
	errx(MOUNT_EX_USAGE, _("only root can do that"));
}