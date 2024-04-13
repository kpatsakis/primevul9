static int run_builtin_or_loaded(struct cli_all_bc *bcs, uint8_t kind, const char* builtin_cbc, struct cli_bc_ctx *ctx, const char *desc)
{
    unsigned i, builtin = 0, rc = 0;
    struct cli_bc *bc = NULL;

    for (i=0;i<bcs->count;i++) {
	bc = &bcs->all_bcs[i];
	if (bc->kind == kind)
	    break;
    }
    if (i == bcs->count)
	bc = NULL;
    if (!bc) {
	/* no loaded bytecode found, load the builtin one! */
	struct cli_dbio dbio;
	bc = cli_calloc(1, sizeof(*bc));
	if (!bc) {
	    cli_errmsg("Out of memory allocating bytecode\n");
	    return CL_EMEM;
	}
	builtin = 1;

	memset(&dbio, 0, sizeof(dbio));
	dbio.usebuf = 1;
	dbio.bufpt = dbio.buf = (char*)builtin_cbc;
	dbio.bufsize = strlen(builtin_cbc)+1;
	if (!dbio.bufsize || dbio.bufpt[dbio.bufsize-2] != '\n') {
	    cli_errmsg("Invalid builtin bytecode: missing terminator\n");
	    free(bc);
	    return CL_EMALFDB;
	}

	rc = cli_bytecode_load(bc, NULL, &dbio, 1);
	if (rc) {
	    cli_errmsg("Failed to load builtin %s bytecode\n", desc);
	    free(bc);
	    return rc;
	}
    }
    rc = cli_bytecode_prepare_interpreter(bc);
    if (rc) {
	cli_errmsg("Failed to prepare %s %s bytecode for interpreter: %s\n",
		   builtin ? "builtin" : "loaded", desc, cl_strerror(rc));
    }
    if (bc->state != bc_interp) {
	cli_errmsg("Failed to prepare %s %s bytecode for interpreter\n",
		   builtin ? "builtin" : "loaded", desc);
	rc = CL_EMALFDB;
    }
    if (!rc) {
	cli_bytecode_context_setfuncid(ctx, bc, 0);
	cli_dbgmsg("Bytecode: %s running (%s)\n", desc,
		   builtin ? "builtin" : "loaded");
	rc = cli_bytecode_run(bcs, bc, ctx);
    }
    if (rc) {
	cli_errmsg("Failed to execute %s %s bytecode: %s\n",builtin ? "builtin":"loaded",
		   desc, cl_strerror(rc));
    }
    if (builtin) {
	cli_bytecode_destroy(bc);
	free(bc);
    }
    return rc;
}