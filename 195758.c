int cli_bytecode_context_setparam_ptr(struct cli_bc_ctx *ctx, unsigned i, void *data, unsigned datalen)
{
    cli_errmsg("Pointer parameters are not implemented yet!\n");
    return CL_EARG;
}