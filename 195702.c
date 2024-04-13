int cli_bytecode_done(struct cli_all_bc *allbc)
{
    return cli_bytecode_done_jit(allbc, 0);
}