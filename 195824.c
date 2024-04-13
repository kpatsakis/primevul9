uint32_t cli_bcapi_engine_scan_options(struct cli_bc_ctx *ctx)
{
    cli_ctx *cctx = (cli_ctx*)ctx->ctx;
    return cctx->options;
}