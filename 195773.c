int32_t cli_bcapi_ilog2(struct cli_bc_ctx *ctx, uint32_t a, uint32_t b)
{
    double f;
    if (!b)
	return 0x7fffffff;
    /* log(a/b) is -32..32, so 2^26*32=2^31 covers the entire range of int32 */
    f = (1<<26)*log((double)a / b) / log(2);
    return (int32_t)myround(f);
}