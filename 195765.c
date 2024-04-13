int32_t cli_bcapi_write(struct cli_bc_ctx *ctx, uint8_t*data, int32_t len)
{
    char err[128];
    int32_t res;

    cli_ctx *cctx = (cli_ctx*)ctx->ctx;
    if (len < 0) {
	cli_warnmsg("Bytecode API: called with negative length!\n");
	API_MISUSE();
	return -1;
    }
    if (!ctx->outfd) {
	ctx->tempfile = cli_gentemp(cctx ? cctx->engine->tmpdir : NULL);
	if (!ctx->tempfile) {
	    cli_dbgmsg("Bytecode API: Unable to allocate memory for tempfile\n");
	    cli_event_error_oom(EV, 0);
	    return -1;
	}
	ctx->outfd = open(ctx->tempfile, O_RDWR|O_CREAT|O_EXCL|O_TRUNC|O_BINARY, 0600);
	if (ctx->outfd == -1) {
	    ctx->outfd = 0;
	    cli_warnmsg("Bytecode API: Can't create file %s: %s\n", ctx->tempfile, cli_strerror(errno, err, sizeof(err)));
	    cli_event_error_str(EV, "cli_bcapi_write: Can't create temporary file");
	    free(ctx->tempfile);
	    return -1;
	}
	cli_dbgmsg("bytecode opened new tempfile: %s\n", ctx->tempfile);
    }

    cli_event_fastdata(ctx->bc_events, BCEV_WRITE, data, len);
    if (cli_checklimits("bytecode api", cctx, ctx->written + len, 0, 0))
	return -1;
    res = cli_writen(ctx->outfd, data, len);
    if (res > 0) ctx->written += res;
    if (res == -1) {
	cli_warnmsg("Bytecode API: write failed: %s\n", cli_strerror(errno, err, sizeof(err)));
	cli_event_error_str(EV, "cli_bcapi_write: write failed");
    }
    return res;
}