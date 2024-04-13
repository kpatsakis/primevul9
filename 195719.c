static int run_selfcheck(struct cli_all_bc *bcs)
{
    struct cli_bc_ctx *ctx;
    struct cli_bc *bc = &bcs->all_bcs[bcs->count-1];
    int rc;
    if (bc->state != bc_jit && bc->state != bc_interp) {
	cli_errmsg("Failed to prepare selfcheck bytecode\n");
	return CL_EBYTECODE;
    }
    ctx = cli_bytecode_context_alloc();
    if (!ctx) {
	cli_errmsg("Failed to allocate bytecode context\n");
	return CL_EMEM;
    }
    cli_bytecode_context_setfuncid(ctx, bc, 0);

    cli_dbgmsg("bytecode self test running\n");
    ctx->bytecode_timeout = 0;
    rc = cli_bytecode_run(bcs, bc, ctx);
    cli_bytecode_context_destroy(ctx);
    if (rc != CL_SUCCESS) {
	cli_errmsg("bytecode self test failed: %s\n",
		   cl_strerror(rc));
    } else {
	cli_dbgmsg("bytecode self test succeeded\n");
    }
    return rc;
}