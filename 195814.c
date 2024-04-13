int32_t cli_bcapi_jsnorm_process(struct cli_bc_ctx *ctx, int32_t id)
{
    unsigned avail;
    unsigned char *in;
    cli_ctx *cctx = ctx->ctx;
    struct bc_jsnorm *b = get_jsnorm(ctx, id);
    if (!b || b->from == -1 || !b->state)
	return -1;

    avail = cli_bcapi_buffer_pipe_read_avail(ctx, b->from);
    in = cli_bcapi_buffer_pipe_read_get(ctx, b->from, avail);
    if (!avail || !in)
	return -1;
    if (cctx && cli_checklimits("bytecode js api", cctx, ctx->jsnormwritten + avail, 0, 0))
	return -1;
    cli_bcapi_buffer_pipe_read_stopped(ctx, b->from, avail);
    cli_js_process_buffer(b->state, (char*)in, avail);
    return 0;
}