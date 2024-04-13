int32_t cli_bcapi_pdf_get_offset(struct cli_bc_ctx *ctx , int32_t objidx)
{
    if (!ctx->pdf_phase ||
	objidx >= ctx->pdf_nobjs)
	return -1;
    return ctx->pdf_startoff + ctx->pdf_objs[objidx].start;
}