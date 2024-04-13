static int cli_bytecode_context_reset(struct cli_bc_ctx *ctx)
{
    unsigned i;

    free(ctx->opsizes);
    ctx->opsizes = NULL;

    free(ctx->values);
    ctx->values = NULL;

    free(ctx->operands);
    ctx->operands = NULL;

    if (ctx->outfd) {
	cli_ctx *cctx = ctx->ctx;
	if (ctx->outfd)
	    close(ctx->outfd);
	if (ctx->tempfile && (!cctx || !cctx->engine->keeptmp)) {
	    cli_unlink(ctx->tempfile);
	}
	free(ctx->tempfile);
	ctx->tempfile = NULL;
	ctx->outfd = 0;
    }
    if (ctx->jsnormdir) {
	char fullname[1025];
	cli_ctx *cctx = ctx->ctx;
	int fd, ret = CL_CLEAN;

	if (!ctx->found) {
	    snprintf(fullname, 1024, "%s"PATHSEP"javascript", ctx->jsnormdir);
	    fd = open(fullname, O_RDONLY|O_BINARY);
	    if(fd >= 0) {
		ret = cli_scandesc(fd, cctx, CL_TYPE_HTML, 0, NULL, AC_SCAN_VIR, NULL);
		if (ret == CL_CLEAN) {
		    lseek(fd, 0, SEEK_SET);
		    ret = cli_scandesc(fd, cctx, CL_TYPE_TEXT_ASCII, 0, NULL, AC_SCAN_VIR, NULL);
		}
		close(fd);
	    }
	}
	if (!cctx || !cctx->engine->keeptmp) {
	    cli_rmdirs(ctx->jsnormdir);
	}
	free(ctx->jsnormdir);
	if (ret != CL_CLEAN)
	    ctx->found = 1;
    }
    ctx->numParams = 0;
    ctx->funcid = 0;
    /* don't touch fmap, file_size, and hooks, sections, ctx, timeout, pdf* */
    ctx->off = 0;
    ctx->written = 0;
    ctx->jsnormwritten = 0;
#if USE_MPOOL
    if (ctx->mpool) {
	mpool_destroy(ctx->mpool);
	ctx->mpool = NULL;
    }
#else
    /*TODO: implement for no-mmap case too*/
#endif
    for (i=0;i<ctx->ninflates;i++)
	cli_bcapi_inflate_done(ctx, i);
    free(ctx->inflates);
    ctx->inflates = NULL;
    ctx->ninflates = 0;

    for (i=0;i<ctx->nbuffers;i++)
	cli_bcapi_buffer_pipe_done(ctx, i);
    free(ctx->buffers);
    ctx->buffers = NULL;
    ctx->nbuffers = 0;

    for (i=0;i<ctx->nhashsets;i++)
	cli_bcapi_hashset_done(ctx, i);
    free(ctx->hashsets);
    ctx->hashsets = NULL;
    ctx->nhashsets = 0;

    for (i=0;i<ctx->njsnorms;i++)
	cli_bcapi_jsnorm_done(ctx, i);
    free(ctx->jsnorms);
    ctx->jsnorms = NULL;
    ctx->njsnorms = 0;
    ctx->jsnormdir = NULL;

    for (i=0;i<ctx->nmaps;i++)
	cli_bcapi_map_done(ctx, i);
    free(ctx->maps);
    ctx->maps = NULL;
    ctx->nmaps = 0;

    ctx->containertype = CL_TYPE_ANY;
    return CL_SUCCESS;
}