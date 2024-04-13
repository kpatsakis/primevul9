static int set_mode(struct cl_engine *engine, enum bytecode_mode mode)
{
    if (engine->bytecode_mode == mode)
	return 0;
    if (engine->bytecode_mode == CL_BYTECODE_MODE_OFF) {
	cli_errmsg("bytecode: already turned off, can't turn it on again!\n");
	return -1;
    }
    cli_dbgmsg("Bytecode: mode changed to %d\n", mode);
    if (engine->bytecode_mode == CL_BYTECODE_MODE_TEST) {
	if (mode == CL_BYTECODE_MODE_OFF || have_clamjit) {
	    cli_errmsg("bytecode: in test mode but JIT/bytecode is about to be disabled: %d\n", mode);
	    engine->bytecode_mode = mode;
	    return -1;
	}
	return 0;
    }
    if (engine->bytecode_mode == CL_BYTECODE_MODE_JIT) {
	cli_errmsg("bytecode: in JIT mode but JIT is about to be disabled: %d\n", mode);
	engine->bytecode_mode = mode;
	return -1;
    }
    engine->bytecode_mode = mode;
    return 0;
}