int32_t cli_bcapi_seek(struct cli_bc_ctx* ctx, int32_t pos, uint32_t whence)
{
    off_t off;
    if (!ctx->fmap) {
	cli_dbgmsg("bcapi_seek: no fmap\n");
	API_MISUSE();
	return -1;
    }
    switch (whence) {
	case 0:
	    off = pos;
	    break;
	case 1:
	    off = ctx->off + pos;
	    break;
	case 2:
	    off = ctx->file_size + pos;
	    break;
	default:
	    API_MISUSE();
	    cli_dbgmsg("bcapi_seek: invalid whence value\n");
	    return -1;
    }
    if (off < 0 || off > ctx->file_size) {
	cli_dbgmsg("bcapi_seek: out of file: %ld (max %d)\n",
		   off, ctx->file_size);
	return -1;
    }
    cli_event_int(EV, BCEV_OFFSET, off);
    ctx->off = off;
    return off;
}