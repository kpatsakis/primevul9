int32_t cli_bcapi_pdf_lookupobj(struct cli_bc_ctx *ctx , uint32_t objid)
{
    unsigned i;
    if (!ctx->pdf_phase)
	return -1;
    for (i=0;i<ctx->pdf_nobjs;i++) {
	if (ctx->pdf_objs[i].id == objid)
	    return i;
    }
    return -1;
}