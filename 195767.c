uint32_t cli_bcapi_disable_jit_if(struct cli_bc_ctx *ctx , const int8_t* reason, uint32_t len, uint32_t cond)
{
    if (ctx->bc->kind != BC_STARTUP) {
	cli_dbgmsg("Bytecode must be BC_STARTUP to call disable_jit_if\n");
	return -1;
    }
    if (!cond)
	return ctx->bytecode_disable_status;
    if (*reason == '^')
	cli_warnmsg("Bytecode: disabling JIT because %s\n", reason+1);
    else
	cli_dbgmsg("Bytecode: disabling JIT because %s\n", reason);
    if (ctx->bytecode_disable_status != 2) /* no reenabling */
	ctx->bytecode_disable_status = 1;
    return ctx->bytecode_disable_status;
}