static void legacy_fs_context_free(struct fs_context *fc)
{
	struct legacy_fs_context *ctx = fc->fs_private;

	if (ctx) {
		if (ctx->param_type == LEGACY_FS_INDIVIDUAL_PARAMS)
			kfree(ctx->legacy_data);
		kfree(ctx);
	}
}