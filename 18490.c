handle_sign_helper(context *ctx, struct pollfd *pollfd, socklen_t size,
		   int attached, bool with_file_type)
{
	int rc = cms_context_alloc(&ctx->cms);
	if (rc < 0)
		return;

	steal_from_cms(ctx->backup_cms, ctx->cms);

	handle_signing(ctx, pollfd, size, attached, with_file_type);

	hide_stolen_goods_from_cms(ctx->cms, ctx->backup_cms);
	cms_context_fini(ctx->cms);
}