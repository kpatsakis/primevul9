int32_t cli_bcapi_pdf_setobjflags(struct cli_bc_ctx *ctx , int32_t objidx, int32_t flags)
{
    if (!ctx->pdf_phase ||
	objidx >= ctx->pdf_nobjs)
	return -1;
    cli_dbgmsg("cli_pdf: bytecode setobjflags %08x -> %08x\n",
	       ctx->pdf_objs[objidx].flags,
	       flags);
    ctx->pdf_objs[objidx].flags = flags;
    return 0;
}