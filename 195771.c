int32_t cli_bcapi_get_pe_section(struct cli_bc_ctx *ctx, struct cli_exe_section* section, uint32_t num)
{
    if (num < ctx->hooks.pedata->nsections) {
	memcpy(section, &ctx->sections[num], sizeof(*section));
	return 0;
    }
    return -1;
}