int cli_bytecode_context_setpdf(struct cli_bc_ctx *ctx, unsigned phase,
				unsigned nobjs,
				struct pdf_obj *objs, uint32_t *pdf_flags,
				uint32_t pdfsize, uint32_t pdfstartoff)
{
    ctx->pdf_nobjs = nobjs;
    ctx->pdf_objs = objs;
    ctx->pdf_flags = pdf_flags;
    ctx->pdf_size = pdfsize;
    ctx->pdf_startoff = pdfstartoff;
    ctx->pdf_phase = phase;
    return 0;
}