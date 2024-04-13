int32_t cli_bcapi_pdf_get_obj_num(struct cli_bc_ctx *ctx)
{
    if (!ctx->pdf_phase)
	return -1;
    return ctx->pdf_nobjs;
}