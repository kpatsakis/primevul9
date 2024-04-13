static int legacy_fs_context_dup(struct fs_context *fc, struct fs_context *src_fc)
{
	struct legacy_fs_context *ctx;
	struct legacy_fs_context *src_ctx = src_fc->fs_private;

	ctx = kmemdup(src_ctx, sizeof(*src_ctx), GFP_KERNEL);
	if (!ctx)
		return -ENOMEM;

	if (ctx->param_type == LEGACY_FS_INDIVIDUAL_PARAMS) {
		ctx->legacy_data = kmemdup(src_ctx->legacy_data,
					   src_ctx->data_size, GFP_KERNEL);
		if (!ctx->legacy_data) {
			kfree(ctx);
			return -ENOMEM;
		}
	}

	fc->fs_private = ctx;
	return 0;
}