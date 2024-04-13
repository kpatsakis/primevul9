uint32_t cli_bcapi_trace_op(struct cli_bc_ctx *ctx, const uint8_t *op, uint32_t col)
{
    if (LIKELY(ctx->trace_level < trace_col))
	return 0;
    if (ctx->trace_level&0xc0) {
	ctx->col = col;
	/* func/scope changed and they needed param/location event */
	ctx->trace(ctx, (ctx->trace_level&0x80) ? trace_func : trace_scope);
	ctx->trace_level &= ~0xc0;
    }
    if (LIKELY(ctx->trace_level < trace_col))
	return 0;
    if (ctx->col != col) {
	ctx->col = col;
	ctx->trace(ctx, trace_col);
    } else {
	ctx->trace(ctx, trace_line);
    }
    if (LIKELY(ctx->trace_level < trace_op))
	return 0;
    if (ctx->trace_op && op)
	ctx->trace_op(ctx, (const char*)op);
    return 0;
}