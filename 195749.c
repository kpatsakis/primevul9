int32_t cli_bcapi_pdf_set_flags(struct cli_bc_ctx *ctx , int32_t flags)
{
    if (!ctx->pdf_phase)
	return -1;
    cli_dbgmsg("cli_pdf: bytecode set_flags %08x -> %08x\n",
	       *ctx->pdf_flags,
	       flags);
    *ctx->pdf_flags = flags;
    return 0;
}