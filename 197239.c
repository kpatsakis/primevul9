is_fstab_entry_mounted(struct mntentchn *mc, int verbose)
{
	struct stat st;

	if (mounted(mc->m.mnt_fsname, mc->m.mnt_dir, mc))
		goto yes;

	/* extra care for loop devices */
	if ((mc->m.mnt_opts && strstr(mc->m.mnt_opts, "loop=")) ||
	    (stat(mc->m.mnt_fsname, &st) == 0 && S_ISREG(st.st_mode))) {

		char *p = get_option_value(mc->m.mnt_opts, "offset=");
		uintmax_t offset = 0;

		if (p && strtosize(p, &offset) != 0) {
			if (verbose)
				printf(_("mount: ignore %s "
					"(unparsable offset= option)\n"),
					mc->m.mnt_fsname);
			return -1;
		}
		free(p);
		if (is_mounted_same_loopfile(mc->m.mnt_dir, mc->m.mnt_fsname, offset))
			goto yes;
	}

	return 0;
yes:
	if (verbose)
		printf(_("mount: %s already mounted on %s\n"),
			       mc->m.mnt_fsname, mc->m.mnt_dir);
	return 1;
}