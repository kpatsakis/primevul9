static int umount_recursive(struct libmnt_context *cxt, const char *spec)
{
	struct libmnt_table *tb;
	int rc;

	/* it's always real mountpoint, don't assume that the target maybe a device */
	mnt_context_disable_swapmatch(cxt, 1);

	tb = mnt_new_table();
	if (!tb)
		err(MOUNT_EX_SYSERR, _("libmount table allocation failed"));
	mnt_table_set_parser_errcb(tb, table_parser_errcb);

	mnt_table_set_cache(tb, mnt_context_get_cache(cxt));

	/*
	 * Don't use mtab here. The recursive umount depends on child-parent
	 * relationship defined by mountinfo file.
	 */
	if (mnt_table_parse_file(tb, _PATH_PROC_MOUNTINFO)) {
		warn(_("failed to parse %s"), _PATH_PROC_MOUNTINFO);
		rc = MOUNT_EX_SOFTWARE;
	} else {
		struct libmnt_fs *fs;

		fs = mnt_table_find_target(tb, spec, MNT_ITER_BACKWARD);
		if (fs)
			rc = umount_do_recurse(cxt, tb, fs);
		else {
			rc = MOUNT_EX_USAGE;
			warnx(access(spec, F_OK) == 0 ?
					_("%s: not mounted") :
					_("%s: not found"), spec);
		}
	}

	mnt_free_table(tb);
	return rc;
}