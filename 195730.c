int32_t cli_bcapi_get_file_reliability(struct cli_bc_ctx *ctx )
{
    cli_ctx *cctx = (cli_ctx*)ctx->ctx;
    return cctx ? cctx->corrupted_input : 3;
}