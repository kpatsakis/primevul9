uint32_t cli_bcapi_setvirusname(struct cli_bc_ctx* ctx, const uint8_t *name, uint32_t len)
{
    ctx->virname = (const char*)name;
    return 0;
}