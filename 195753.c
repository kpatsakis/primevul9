uint32_t cli_bcapi_pe_rawaddr(struct cli_bc_ctx *ctx, uint32_t rva)
{
  uint32_t ret;
  unsigned err = 0;
  const struct cli_pe_hook_data *pe = ctx->hooks.pedata;
  ret = cli_rawaddr(rva, ctx->sections, pe->nsections, &err,
		    ctx->file_size, pe->hdr_size);
  if (err) {
    cli_dbgmsg("bcapi_pe_rawaddr invalid rva: %u\n", rva);
    return PE_INVALID_RVA;
  }
  return ret;
}