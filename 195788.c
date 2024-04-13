int32_t cli_bcapi_read_number(struct cli_bc_ctx *ctx, uint32_t radix)
{
    unsigned i;
    char *p;
    int32_t result;

    if ((radix != 10 && radix != 16) || !ctx->fmap)
	return -1;
    cli_event_int(EV, BCEV_OFFSET, ctx->off);
    while ((p = fmap_need_off_once(ctx->fmap, ctx->off, BUF))) {
	for (i=0;i<BUF;i++) {
	    if (p[i] >= '0' && p[i] <= '9') {
		char *endptr;
		p = fmap_need_ptr_once(ctx->fmap, p+i, 16);
		if (!p)
		    return -1;
		result = strtoul(p, &endptr, radix);
		ctx->off += i + (endptr - p);
		return result;
	    }
	}
	ctx->off += BUF;
    }
    return -1;
}