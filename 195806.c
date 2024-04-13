uint32_t cli_bcapi_get_environment(struct cli_bc_ctx *ctx , struct cli_environment* env, uint32_t len)
{
    if (len > sizeof(*env)) {
	cli_dbgmsg("cli_bcapi_get_environment len %u > %lu\n", len, sizeof(*env));
	return -1;
    }
    memcpy(env, ctx->env, len);
    return 0;
}