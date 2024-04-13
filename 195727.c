uint32_t cli_bcapi_trace_source(struct cli_bc_ctx *ctx, const uint8_t *file, uint32_t line)
{
    if (LIKELY(ctx->trace_level < trace_line))
	return 0;
    if (ctx->file != (const char*)file || ctx->line != line) {
	ctx->col = 0;
	ctx->file =(const char*)file ? (const char*)file : "??";
	ctx->line = line;
    }
    return 0;
}