uint32_t cli_bcapi_test2(struct cli_bc_ctx *ctx, uint32_t a)
{
    return a == 0xf00d ? 0xd00f : 0x5555;
}