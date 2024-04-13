uint32_t cli_bcapi_trace_directory(struct cli_bc_ctx *ctx, const uint8_t* dir, uint32_t dummy)
{
    if (LIKELY(!ctx->trace_level))
	return 0;
    ctx->directory = (const char*)dir ? (const char*)dir : "";
    return 0;
}