uint8_t* cli_bcapi_malloc(struct cli_bc_ctx *ctx, uint32_t size)
{
    void *v;
#if USE_MPOOL
    if (!ctx->mpool) {
	ctx->mpool = mpool_create();
	if (!ctx->mpool) {
	    cli_dbgmsg("bytecode: mpool_create failed!\n");
	    cli_event_error_oom(EV, 0);
	    return NULL;
	}
    }
    v = mpool_malloc(ctx->mpool, size);
#else
    /* TODO: implement using a list of pointers we allocated! */
    cli_errmsg("cli_bcapi_malloc not implemented for systems without mmap yet!\n");
    v = cli_malloc(size);
#endif
    if (!v)
	cli_event_error_oom(EV, size);
    return v;
}