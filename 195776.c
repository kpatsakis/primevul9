uint32_t cli_bcapi_disable_bytecode_if(struct cli_bc_ctx *ctx , const int8_t* reason, uint32_t len, uint32_t cond)
{
    if (ctx->bc->kind != BC_STARTUP) {
	cli_dbgmsg("Bytecode must be BC_STARTUP to call disable_bytecode_if\n");
	return -1;
    }
    if (!cond)
	return ctx->bytecode_disable_status;
    if (*reason == '^')
	cli_warnmsg("Bytecode: disabling completely because %s\n", reason+1);
    else
	cli_dbgmsg("Bytecode: disabling completely because %s\n", reason);
    ctx->bytecode_disable_status = 2;
    return ctx->bytecode_disable_status;
}