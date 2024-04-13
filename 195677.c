uint8_t* cli_bcapi_pdf_getobj(struct cli_bc_ctx *ctx , int32_t objidx, uint32_t amount)
{
    uint32_t size = cli_bcapi_pdf_getobjsize(ctx, objidx);
    if (amount > size)
	return NULL;
    return fmap_need_off(ctx->fmap, ctx->pdf_objs[objidx].start, amount);
}