static int mount_all(struct libmnt_context *cxt)
{
	struct libmnt_iter *itr;
	struct libmnt_fs *fs;
	int mntrc, ignored, rc = MOUNT_EX_SUCCESS;

	int nsucc = 0, nerrs = 0;

	itr = mnt_new_iter(MNT_ITER_FORWARD);
	if (!itr) {
		warn(_("failed to initialize libmount iterator"));
		return MOUNT_EX_SYSERR;
	}

	while (mnt_context_next_mount(cxt, itr, &fs, &mntrc, &ignored) == 0) {

		const char *tgt = mnt_fs_get_target(fs);

		if (ignored) {
			if (mnt_context_is_verbose(cxt))
				printf(ignored == 1 ? _("%-25s: ignored\n") :
						      _("%-25s: already mounted\n"),
						tgt);
		} else if (mnt_context_is_fork(cxt)) {
			if (mnt_context_is_verbose(cxt))
				printf("%-25s: mount successfully forked\n", tgt);
		} else {
			mk_exit_code(cxt, mntrc);	/* to print warnings */

			if (mnt_context_get_status(cxt)) {
				nsucc++;

				if (mnt_context_is_verbose(cxt))
					printf("%-25s: successfully mounted\n", tgt);
			} else
				nerrs++;
		}
	}

	if (mnt_context_is_parent(cxt)) {
		/* wait for mount --fork children */
		int nchildren = 0;

		nerrs = 0, nsucc = 0;

		rc = mnt_context_wait_for_children(cxt, &nchildren, &nerrs);
		if (!rc && nchildren)
			nsucc = nchildren - nerrs;
	}

	if (nerrs == 0)
		rc = MOUNT_EX_SUCCESS;		/* all success */
	else if (nsucc == 0)
		rc = MOUNT_EX_FAIL;		/* all failed */
	else
		rc = MOUNT_EX_SOMEOK;		/* some success, some failed */

	mnt_free_iter(itr);
	return rc;
}