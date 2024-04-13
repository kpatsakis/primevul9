static struct libmnt_table *append_fstab(struct libmnt_context *cxt,
					 struct libmnt_table *fstab,
					 const char *path)
{

	if (!fstab) {
		fstab = mnt_new_table();
		if (!fstab)
			err(MOUNT_EX_SYSERR, _("failed to initialize libmount table"));

		mnt_table_set_parser_errcb(fstab, table_parser_errcb);
		mnt_context_set_fstab(cxt, fstab);
	}

	if (mnt_table_parse_fstab(fstab, path))
		errx(MOUNT_EX_USAGE,_("%s: failed to parse"), path);

	return fstab;
}