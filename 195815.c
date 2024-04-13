void cli_bytecode_context_setctx(struct cli_bc_ctx *ctx, void *cctx)
{
    ctx->ctx = cctx;
    ctx->bytecode_timeout = ((cli_ctx*)cctx)->engine->bytecode_timeout;
}