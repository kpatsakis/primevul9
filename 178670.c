static int umount_all(struct libmnt_context *cxt)
{
	struct libmnt_iter *itr;
	struct libmnt_fs *fs;
	int mntrc, ignored, rc = 0;

	itr = mnt_new_iter(MNT_ITER_BACKWARD);
	if (!itr) {
		warn(_("failed to initialize libmount iterator"));
		return MOUNT_EX_SYSERR;
	}

	while (mnt_context_next_umount(cxt, itr, &fs, &mntrc, &ignored) == 0) {

		const char *tgt = mnt_fs_get_target(fs);

		if (ignored) {
			if (mnt_context_is_verbose(cxt))
				printf(_("%-25s: ignored\n"), tgt);
		} else {
			rc |= mk_exit_code(cxt, mntrc);

			if (mnt_context_is_verbose(cxt))
				printf("%-25s: successfully umounted\n", tgt);
		}
	}

	mnt_free_iter(itr);
	return rc;
}