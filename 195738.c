int32_t cli_bcapi_bytecode_rt_error(struct cli_bc_ctx *ctx , int32_t id)
{
    int32_t line = id >> 8;
    int32_t col = id&0xff;
    cli_warnmsg("Bytecode runtime error at line %u, col %u\n", line, col);
    return 0;
}