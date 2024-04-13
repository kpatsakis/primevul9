uint32_t cli_bcapi_check_platform(struct cli_bc_ctx *ctx , uint32_t a, uint32_t b , uint32_t c)
{
    unsigned ret =
	check_bits(a, ctx->env->platform_id_a, 24, 0xff) &&
	check_bits(a, ctx->env->platform_id_a, 20, 0xf) &&
	check_bits(a, ctx->env->platform_id_a, 16, 0xf) &&
	check_bits(a, ctx->env->platform_id_a, 8, 0xff) &&
	check_bits(a, ctx->env->platform_id_a, 0, 0xff) &&
	check_bits(b, ctx->env->platform_id_b, 28, 0xf) &&
	check_bits(b, ctx->env->platform_id_b, 24, 0xf) &&
	check_bits(b, ctx->env->platform_id_b, 16, 0xff) &&
	check_bits(b, ctx->env->platform_id_b, 8, 0xff) &&
	check_bits(b, ctx->env->platform_id_b, 0, 0xff) &&
	check_bits(c, ctx->env->platform_id_c, 24, 0xff) &&
	check_bits(c, ctx->env->platform_id_c, 16, 0xff) &&
	check_bits(c, ctx->env->platform_id_c, 8, 0xff) &&
	check_bits(c, ctx->env->platform_id_c, 0, 0xff);
    if (ret) {
	cli_dbgmsg("check_platform(0x%08x,0x%08x,0x%08x) = match\n",a,b,c);
    }
    return ret;
}