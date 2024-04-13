static int umount_do_recurse(struct libmnt_context *cxt,
		struct libmnt_table *tb, struct libmnt_fs *parent)
{
	int rc, mounted = 0;
	struct libmnt_fs *child;
	const char *target = mnt_fs_get_target(parent);
	struct libmnt_iter *itr = mnt_new_iter(MNT_ITER_BACKWARD);

	if (!itr)
		err(MOUNT_EX_SYSERR, _("libmount iterator allocation failed"));
	/*
	 * umount all childern
	 */
	for (;;) {
		rc = mnt_table_next_child_fs(tb, itr, parent, &child);
		if (rc < 0) {
			warnx(_("failed to get child fs of %s"), target);
			rc = MOUNT_EX_SOFTWARE;
			goto done;
		} else if (rc == 1)
			break;		/* no more children */

		rc = umount_do_recurse(cxt, tb, child);
		if (rc != MOUNT_EX_SUCCESS)
			goto done;
	}


	/*
	 * Let's check if the pointpoint is still mounted -- for example with
	 * shared subtrees maybe the mountpoint already unmounted by any
	 * previous umount(2) call.
	 *
	 * Note that here we a little duplicate code from umount_one() and
	 * mnt_context_umount(). It's no problem to call
	 * mnt_context_prepare_umount() more than once. This solution is better
	 * than directly call mnt_context_is_fs_mounted(), because libmount is
	 * able to optimize mtab usage by mnt_context_set_tabfilte().
	 */
	if (mnt_context_set_target(cxt, mnt_fs_get_target(parent)))
		err(MOUNT_EX_SYSERR, _("failed to set umount target"));

	rc = mnt_context_prepare_umount(cxt);
	if (!rc)
		rc = mnt_context_is_fs_mounted(cxt, parent, &mounted);
	if (mounted)
		rc = umount_one(cxt, target);
	else {
		if (rc)
			rc = mk_exit_code(cxt, rc);	/* error */
		else
			rc = MOUNT_EX_SUCCESS;		/* alredy unmounted */
		mnt_reset_context(cxt);
	}
done:
	mnt_free_iter(itr);
	return rc;
}