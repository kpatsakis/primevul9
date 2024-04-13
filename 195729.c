uint32_t cli_bcapi_trace_value(struct cli_bc_ctx *ctx, const uint8_t* name, uint32_t value)
{
    if (LIKELY(ctx->trace_level < trace_val))
	return 0;
    if (ctx->trace_level&0x80) {
	if ((ctx->trace_level&0x7f) < trace_param)
	    return 0;
	ctx->trace(ctx, trace_param);
    }
    if (ctx->trace_val && name)
	ctx->trace_val(ctx, (const char*)name, value);
    return 0;
}