int32_t cli_bcapi_ipow(struct cli_bc_ctx *ctx, int32_t a, int32_t b, int32_t c)
{
    if (!a && b < 0)
	return 0x7fffffff;
    return (int32_t)myround(c*pow(a,b));
}