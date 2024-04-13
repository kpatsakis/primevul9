uint64_t cli_bytecode_context_getresult_int(struct cli_bc_ctx *ctx)
{
    return *(uint32_t*)ctx->values;/*XXX*/
}