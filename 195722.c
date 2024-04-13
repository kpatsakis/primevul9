static int selfcheck(int jit, struct cli_bcengine *engine)
{
    struct cli_all_bc bcs;
    int rc;

    memset(&bcs, 0, sizeof(bcs));
    bcs.all_bcs = NULL;
    bcs.count = 0;
    bcs.engine = engine;
    rc = add_selfcheck(&bcs);
    if (rc == CL_SUCCESS) {
	if (jit) {
	    if (!bcs.engine) {
		cli_dbgmsg("bytecode: JIT disabled\n");
		rc = CL_BREAK;/* no JIT - not fatal */
	    } else {
		rc = cli_bytecode_prepare_jit(&bcs);
	    }
	} else {
	    rc = cli_bytecode_prepare_interpreter(bcs.all_bcs);
	}
	if (rc == CL_SUCCESS)
	    rc = run_selfcheck(&bcs);
	if (rc == CL_BREAK)
	    rc = CL_SUCCESS;
    }
    cli_bytecode_destroy(bcs.all_bcs);
    free(bcs.all_bcs);
    cli_bytecode_done_jit(&bcs, 1);
    if (rc != CL_SUCCESS) {
	cli_errmsg("Bytecode: failed to run selfcheck in %s mode: %s\n",
		   jit ? "JIT" : "interpreter", cl_strerror(rc));
    }
    return rc;
}