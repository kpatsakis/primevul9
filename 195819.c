int32_t cli_bcapi_running_on_jit(struct cli_bc_ctx *ctx )
{
    ctx->no_diff = 1;
    return ctx->on_jit;
}