static void sanitize_paths(struct libmnt_context *cxt)
{
	const char *p;
	struct libmnt_fs *fs = mnt_context_get_fs(cxt);

	if (!fs)
		return;

	p = mnt_fs_get_target(fs);
	if (p) {
		char *np = canonicalize_path_restricted(p);
		if (!np)
			err(MOUNT_EX_USAGE, "%s", p);
		mnt_fs_set_target(fs, np);
		free(np);
	}

	p = mnt_fs_get_srcpath(fs);
	if (p) {
		char *np = canonicalize_path_restricted(p);
		if (!np)
			err(MOUNT_EX_USAGE, "%s", p);
		mnt_fs_set_source(fs, np);
		free(np);
	}
}