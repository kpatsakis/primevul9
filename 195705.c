uint32_t cli_bcapi_trace_ptr(struct cli_bc_ctx *ctx, const uint8_t* ptr, uint32_t dummy)
{
    if (LIKELY(ctx->trace_level < trace_val))
	return 0;
    if (ctx->trace_level&0x80) {
	if ((ctx->trace_level&0x7f) < trace_param)
	    return 0;
	ctx->trace(ctx, trace_param);
    }
    if (ctx->trace_ptr)
	ctx->trace_ptr(ctx, ptr);
    return 0;
}