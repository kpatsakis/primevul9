int32_t cli_bcapi_file_byteat(struct cli_bc_ctx *ctx, uint32_t off)
{
    unsigned char c;
    if (!ctx->fmap) {
	cli_dbgmsg("bcapi_file_byteat: no fmap\n");
	return -1;
    }
    cli_event_int(EV, BCEV_OFFSET, off);
    if (fmap_readn(ctx->fmap, &c, off, 1) != 1) {
	cli_dbgmsg("bcapi_file_byteat: fmap_readn failed at %u\n", off);
	return -1;
    }
    return c;
}