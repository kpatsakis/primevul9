static int umount_one(struct libmnt_context *cxt, const char *spec)
{
	int rc;

	if (!spec)
		return MOUNT_EX_SOFTWARE;

	if (mnt_context_set_target(cxt, spec))
		err(MOUNT_EX_SYSERR, _("failed to set umount target"));

	rc = mnt_context_umount(cxt);
	rc = mk_exit_code(cxt, rc);

	if (rc == MOUNT_EX_SUCCESS && mnt_context_is_verbose(cxt))
		success_message(cxt);

	mnt_reset_context(cxt);
	return rc;
}