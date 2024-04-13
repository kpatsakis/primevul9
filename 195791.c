uint32_t cli_bcapi_pdf_getobjsize(struct cli_bc_ctx *ctx , int32_t objidx)
{
    if (!ctx->pdf_phase ||
	objidx >= ctx->pdf_nobjs ||
	ctx->pdf_phase == PDF_PHASE_POSTDUMP /* map is obj itself, no access to pdf anymore */
       )
	return 0;
    if (objidx + 1 == ctx->pdf_nobjs)
	return ctx->pdf_size - ctx->pdf_objs[objidx].start;
    return ctx->pdf_objs[objidx+1].start - ctx->pdf_objs[objidx].start - 4;
}