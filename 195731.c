uint32_t cli_bcapi_engine_db_options(struct cli_bc_ctx *ctx)
{
    cli_ctx *cctx = (cli_ctx*)ctx->ctx;
    return cctx->engine->dboptions;
}