int32_t cli_bcapi_isin(struct cli_bc_ctx *ctx, int32_t a, int32_t b, int32_t c)
{
    double f;
    if (!b)
	return 0x7fffffff;
    f = c*sin((double)a/b);
    return (int32_t)myround(f);
}