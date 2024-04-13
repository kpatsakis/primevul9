handle_invalid_input(pesignd_cmd cmd, context *ctx,
		     struct pollfd *pollfd UNUSED,
		     socklen_t size UNUSED)
{
		ctx->backup_cms->log(ctx->backup_cms, ctx->priority|LOG_ERR,
			"got unexpected command 0x%x", cmd);
		ctx->backup_cms->log(ctx->backup_cms, ctx->priority|LOG_ERR,
			"possible exploit attempt");
}