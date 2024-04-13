int32_t cli_bcapi_extract_set_container(struct cli_bc_ctx *ctx, uint32_t ftype)
{
    if (ftype > CL_TYPE_IGNORED)
	return -1;
    ctx->containertype = ftype;
    return 0;
}