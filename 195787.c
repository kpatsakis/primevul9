int cli_bytecode_init(struct cli_all_bc *allbc)
{
    int ret;
    memset(allbc, 0, sizeof(*allbc));
    ret = cli_bytecode_init_jit(allbc, 0/*XXX*/);
    cli_dbgmsg("Bytecode initialized in %s mode\n",
	       allbc->engine ? "JIT" : "interpreter");
    allbc->inited = 1;
    return ret;
}