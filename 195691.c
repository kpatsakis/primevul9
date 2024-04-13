int32_t cli_bcapi_file_find_limit(struct cli_bc_ctx *ctx , const uint8_t* data, uint32_t len, int32_t limit)
{
    char buf[4096];
    fmap_t *map = ctx->fmap;
    uint32_t off = ctx->off;
    int n;

    if (!map || len > sizeof(buf)/4 || len <= 0 || limit <= 0) {
	cli_dbgmsg("bcapi_file_find_limit preconditions not met\n");
	API_MISUSE();
	return -1;
    }

    cli_event_int(EV, BCEV_OFFSET, off);
    cli_event_fastdata(EV, BCEV_FIND, data, len);
    for (;;) {
	const char *p;
	int32_t readlen = sizeof(buf);
	if (off + readlen > limit) {
	    readlen = limit - off;
	    if (readlen < 0)
		return -1;
	}
	n = fmap_readn(map, buf, off, readlen);
	if ((unsigned)n < len || n < 0)
	    return -1;
	p = cli_memmem(buf, n, data, len);
	if (p)
	    return off + p - buf;
	off += n;
    }
    return -1;
}