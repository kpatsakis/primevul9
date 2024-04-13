uint32_t cli_bcapi_disasm_x86(struct cli_bc_ctx *ctx, struct DISASM_RESULT *res, uint32_t len)
{
    int n;
    const unsigned char *buf;
    const unsigned char* next;
    if (!res || !ctx->fmap || ctx->off >= ctx->fmap->len) {
	API_MISUSE();
	return -1;
    }
    /* 32 should be longest instr we support decoding.
     * When we'll support mmx/sse instructions this should be updated! */
    n = MIN(32, ctx->fmap->len - ctx->off);
    buf = fmap_need_off_once(ctx->fmap, ctx->off, n);
    next = cli_disasm_one(buf, n, res, 0);
    if (!next) {
	cli_dbgmsg("bcapi_disasm: failed\n");
	cli_event_count(EV, BCEV_DISASM_FAIL);
	return -1;
    }
    return ctx->off + next - buf;
}