int32_t cli_bcapi_read(struct cli_bc_ctx* ctx, uint8_t *data, int32_t size)
{
    int n;
    if (!ctx->fmap) {
	API_MISUSE();
	return -1;
    }
    if (size < 0 || size > CLI_MAX_ALLOCATION) {
	cli_warnmsg("bytecode: negative read size: %d\n", size);
	API_MISUSE();
	return -1;
    }
    n = fmap_readn(ctx->fmap, data, ctx->off, size);
    if (n <= 0) {
	cli_dbgmsg("bcapi_read: fmap_readn failed (requested %d)\n", size);
	cli_event_count(EV, BCEV_READ_ERR);
	return n;
    }
    cli_event_int(EV, BCEV_OFFSET, ctx->off);
    cli_event_fastdata(EV, BCEV_READ, data, size);
    //cli_event_data(EV, BCEV_READ, data, n);
    ctx->off += n;
    return n;
}