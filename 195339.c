static int legacy_parse_monolithic(struct fs_context *fc, void *data)
{
	struct legacy_fs_context *ctx = fc->fs_private;

	if (ctx->param_type != LEGACY_FS_UNSET_PARAMS) {
		pr_warn("VFS: Can't mix monolithic and individual options\n");
		return -EINVAL;
	}

	ctx->legacy_data = data;
	ctx->param_type = LEGACY_FS_MONOLITHIC_PARAMS;
	if (!ctx->legacy_data)
		return 0;

	if (fc->fs_type->fs_flags & FS_BINARY_MOUNTDATA)
		return 0;
	return security_sb_eat_lsm_opts(ctx->legacy_data, &fc->security);
}