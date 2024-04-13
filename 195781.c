uint32_t cli_bcapi_trace_scope(struct cli_bc_ctx *ctx, const uint8_t *scope, uint32_t scopeid)
{
    if (LIKELY(!ctx->trace_level))
	return 0;
    if (ctx->scope != (const char*)scope) {
	ctx->scope = (const char*)scope ? (const char*)scope : "?";
	ctx->scopeid = scopeid;
	ctx->trace_level |= 0x80;/* temporarely increase level to print params */
    } else if ((ctx->trace_level >= trace_scope) && ctx->scopeid != scopeid) {
	ctx->scopeid = scopeid;
	ctx->trace_level |= 0x40;/* temporarely increase level to print location */
    }
    return 0;
}