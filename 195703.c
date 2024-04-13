int32_t cli_bcapi_pdf_get_dumpedobjid(struct cli_bc_ctx *ctx)
{
    if (ctx->pdf_phase != PDF_PHASE_POSTDUMP)
	return -1;
    return ctx->pdf_dumpedid;
}