static void print_all(struct libmnt_context *cxt, char *pattern, int show_label)
{
	struct libmnt_table *tb;
	struct libmnt_iter *itr = NULL;
	struct libmnt_fs *fs;
	struct libmnt_cache *cache = NULL;

	if (mnt_context_get_mtab(cxt, &tb))
		err(MOUNT_EX_SYSERR, _("failed to read mtab"));

	itr = mnt_new_iter(MNT_ITER_FORWARD);
	if (!itr)
		err(MOUNT_EX_SYSERR, _("failed to initialize libmount iterator"));
	if (show_label)
		cache = mnt_new_cache();

	while (mnt_table_next_fs(tb, itr, &fs) == 0) {
		const char *type = mnt_fs_get_fstype(fs);
		const char *src = mnt_fs_get_source(fs);
		const char *optstr = mnt_fs_get_options(fs);
		char *xsrc = NULL;

		if (type && pattern && !mnt_match_fstype(type, pattern))
			continue;

		if (!mnt_fs_is_pseudofs(fs))
			xsrc = mnt_pretty_path(src, cache);
		printf ("%s on ", xsrc ? xsrc : src);
		safe_fputs(mnt_fs_get_target(fs));

		if (type)
			printf (" type %s", type);
		if (optstr)
			printf (" (%s)", optstr);
		if (show_label && src) {
			char *lb = mnt_cache_find_tag_value(cache, src, "LABEL");
			if (lb)
				printf (" [%s]", lb);
		}
		fputc('\n', stdout);
		free(xsrc);
	}

	mnt_free_cache(cache);
	mnt_free_iter(itr);
}