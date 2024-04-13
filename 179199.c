check_fstab(const char *progname, const char *mountpoint, const char *devname,
	    char **options)
{
	FILE *fstab;
	struct mntent *mnt;
	size_t len;

	/* make sure this mount is listed in /etc/fstab */
	fstab = setmntent(_PATH_MNTTAB, "r");
	if (!fstab) {
		fprintf(stderr, "Couldn't open %s for reading!\n", _PATH_MNTTAB);
		return EX_FILEIO;
	}

	while ((mnt = getmntent(fstab))) {
		len = strlen(mnt->mnt_dir);
		while (len > 1) {
		        if (mnt->mnt_dir[len - 1] == '/')
			        mnt->mnt_dir[len - 1] = '\0';
		        else
				break;
			len--;
		}
		if (!strcmp(mountpoint, mnt->mnt_dir))
			break;
	}
	endmntent(fstab);

	if (mnt == NULL || strcmp(mnt->mnt_fsname, devname)) {
		fprintf(stderr, "%s: permission denied: no match for "
			"%s found in %s\n", progname, mountpoint, _PATH_MNTTAB);
		return EX_USAGE;
	}

	/*
	 * 'mount' munges the options from fstab before passing them
	 * to us. It is non-trivial to test that we have the correct
	 * set of options. We don't want to trust what the user
	 * gave us, so just take whatever is in /etc/fstab.
	 */
	*options = strdup(mnt->mnt_opts);
	return 0;
}