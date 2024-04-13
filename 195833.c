int cli_bytecode_context_clear(struct cli_bc_ctx *ctx)
{
    cli_bytecode_context_reset(ctx);
    memset(ctx, 0, sizeof(*ctx));
    return CL_SUCCESS;
}