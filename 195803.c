int32_t cli_bcapi_jsnorm_init(struct cli_bc_ctx *ctx, int32_t from)
{
    struct parser_state *state;
    struct bc_jsnorm *b;
    unsigned  n = ctx->njsnorms + 1;
    if (!get_buffer(ctx, from)) {
	cli_dbgmsg("bytecode api: jsnorm_init: invalid buffers!\n");
	return -1;
    }
    state = cli_js_init();
    if (!state)
	return -1;
    b = cli_realloc(ctx->jsnorms, sizeof(*ctx->jsnorms)*n);
    if (!b) {
	cli_js_destroy(state);
	return -1;
    }
    ctx->jsnorms = b;
    ctx->njsnorms = n;
    b = &b[n-1];
    b->from = from;
    b->state = state;
    if (!ctx->jsnormdir) {
	cli_ctx *cctx = (cli_ctx*)ctx->ctx;
	ctx->jsnormdir = cli_gentemp(cctx ? cctx->engine->tmpdir : NULL);
	if (ctx->jsnormdir) {
	    if (mkdir(ctx->jsnormdir, 0700)) {
		cli_dbgmsg("js: can't create temp dir %s\n", ctx->jsnormdir);
		free(ctx->jsnormdir);
		return CL_ETMPDIR;
	    }
	}
    }
    return n-1;
}