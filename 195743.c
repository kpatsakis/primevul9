int32_t cli_bcapi_fill_buffer(struct cli_bc_ctx *ctx, uint8_t* buf,
			      uint32_t buflen, uint32_t filled,
			      uint32_t pos, uint32_t fill)
{
    int32_t res, remaining, tofill;
    if (!buf || !buflen || buflen > CLI_MAX_ALLOCATION || filled > buflen) {
	cli_dbgmsg("fill_buffer1\n");
	API_MISUSE();
	return -1;
    }
    if (ctx->off >= ctx->file_size) {
	cli_dbgmsg("fill_buffer2\n");
	API_MISUSE();
	return 0;
    }
    remaining = filled - pos;
    if (remaining) {
	if (!CLI_ISCONTAINED(buf, buflen, buf+pos, remaining)) {
	    cli_dbgmsg("fill_buffer3\n");
	    API_MISUSE();
	    return -1;
	}
	memmove(buf, buf+pos, remaining);
    }
    tofill = buflen - remaining;
    if (!CLI_ISCONTAINED(buf, buflen, buf+remaining, tofill)) {
	cli_dbgmsg("fill_buffer4\n");
	API_MISUSE();
	return -1;
    }
    res = cli_bcapi_read(ctx, buf+remaining, tofill);
    if (res <= 0) {
	cli_dbgmsg("fill_buffer5\n");
	API_MISUSE();
	return res;
    }
    return remaining + res;
}