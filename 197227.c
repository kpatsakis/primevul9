create_mtab (void) {
	struct mntentchn *fstab;
	struct my_mntent mnt;
	int flags;
	mntFILE *mfp;

	lock_mtab();

	mfp = my_setmntent (_PATH_MOUNTED, "a+");
	if (mfp == NULL || mfp->mntent_fp == NULL) {
		int errsv = errno;
		die (EX_FILEIO, _("mount: can't open %s for writing: %s"),
		     _PATH_MOUNTED, strerror (errsv));
	}

	/* Find the root entry by looking it up in fstab */
	if ((fstab = getfs_by_dir ("/")) || (fstab = getfs_by_dir ("root"))) {
		char *extra_opts;
		parse_opts (fstab->m.mnt_opts, &flags, &extra_opts);
		mnt.mnt_dir = "/";
		mnt.mnt_fsname = spec_to_devname(fstab->m.mnt_fsname);
		mnt.mnt_type = fstab->m.mnt_type;
		mnt.mnt_opts = fix_opts_string (flags, extra_opts, NULL, 0);
		mnt.mnt_freq = mnt.mnt_passno = 0;
		free(extra_opts);

		if (my_addmntent (mfp, &mnt) == 1) {
			int errsv = errno;
			die (EX_FILEIO, _("mount: error writing %s: %s"),
			     _PATH_MOUNTED, strerror (errsv));
		}
	}
	if (fchmod (fileno (mfp->mntent_fp), 0644) < 0)
		if (errno != EROFS) {
			int errsv = errno;
			die (EX_FILEIO,
			     _("mount: error changing mode of %s: %s"),
			     _PATH_MOUNTED, strerror (errsv));
		}
	my_endmntent (mfp);

	unlock_mtab();

	reset_mtab_info();
}