int32_t cli_bcapi_jsnorm_done(struct cli_bc_ctx *ctx , int32_t id)
{
    struct bc_jsnorm *b = get_jsnorm(ctx, id);
    if (!b || b->from == -1)
	return -1;
    if (ctx->ctx && cli_updatelimits(ctx->ctx, ctx->jsnormwritten))
	return -1;
    ctx->jsnormwritten = 0;
    cli_js_parse_done(b->state);
    cli_js_output(b->state, ctx->jsnormdir);
    cli_js_destroy(b->state);
    b->from = -1;
    return 0;
}