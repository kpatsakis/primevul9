struct cli_bc_ctx *cli_bytecode_context_alloc(void)
{
    struct cli_bc_ctx *ctx = cli_calloc(1, sizeof(*ctx));
    ctx->bytecode_timeout = 60000;
    cli_bytecode_context_reset(ctx);
    return ctx;
}