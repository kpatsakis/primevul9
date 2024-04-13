int finish_clean_context(struct fs_context *fc)
{
	int error;

	if (fc->phase != FS_CONTEXT_AWAITING_RECONF)
		return 0;

	if (fc->fs_type->init_fs_context)
		error = fc->fs_type->init_fs_context(fc);
	else
		error = legacy_init_fs_context(fc);
	if (unlikely(error)) {
		fc->phase = FS_CONTEXT_FAILED;
		return error;
	}
	fc->need_free = true;
	fc->phase = FS_CONTEXT_RECONF_PARAMS;
	return 0;
}