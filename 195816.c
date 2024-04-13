int cli_bytecode_run(const struct cli_all_bc *bcs, const struct cli_bc *bc, struct cli_bc_ctx *ctx)
{
    int ret = CL_SUCCESS;
    struct cli_bc_inst inst;
    struct cli_bc_func func;
    cli_events_t *jit_ev = NULL, *interp_ev = NULL;

    int test_mode = 0;
    cli_ctx *cctx =(cli_ctx*)ctx->ctx;
    if (cctx && cctx->engine->bytecode_mode == CL_BYTECODE_MODE_TEST)
	test_mode = 1;

    if (!ctx || !ctx->bc || !ctx->func)
	return CL_ENULLARG;
    if (ctx->numParams && (!ctx->values || !ctx->operands))
	return CL_ENULLARG;
    if (bc->state == bc_loaded) {
	cli_errmsg("bytecode has to be prepared either for interpreter or JIT!\n");
	return CL_EARG;
    }
    if (bc->state == bc_disabled) {
	cli_dbgmsg("bytecode triggered but running bytecodes is disabled\n");
	return CL_SUCCESS;
    }
    if (cctx)
	cli_event_time_start(cctx->perf, PERFT_BYTECODE);
    ctx->env = &bcs->env;
    context_safe(ctx);
    if (test_mode) {
	jit_ev = cli_events_new(BCEV_LASTEVENT);
	interp_ev = cli_events_new(BCEV_LASTEVENT);
	if (!jit_ev || !interp_ev) {
	    cli_events_free(jit_ev);
	    cli_events_free(interp_ev);
	    return CL_EMEM;
	}
	if (register_events(jit_ev) == -1 ||
	    register_events(interp_ev) == -1) {
	    cli_events_free(jit_ev);
	    cli_events_free(interp_ev);
	    return CL_EBYTECODE_TESTFAIL;
	}
    }
    if (bc->state == bc_interp || test_mode) {
	ctx->bc_events = interp_ev;
	memset(&func, 0, sizeof(func));
	func.numInsts = 1;
	func.numValues = 1;
	func.numConstants = 0;
	func.numBytes = ctx->bytes;
	memset(ctx->values+ctx->bytes-8, 0, 8);

	inst.opcode = OP_BC_CALL_DIRECT;
	inst.interp_op = OP_BC_CALL_DIRECT*5;
	inst.dest = func.numArgs;
	inst.type = 0;
	inst.u.ops.numOps = ctx->numParams;
	inst.u.ops.funcid = ctx->funcid;
	inst.u.ops.ops = ctx->operands;
	inst.u.ops.opsizes = ctx->opsizes;
	cli_dbgmsg("Bytecode %u: executing in interpeter mode\n", bc->id);

	ctx->on_jit = 0;

	cli_event_time_start(interp_ev, BCEV_EXEC_TIME);
	ret = cli_vm_execute(ctx->bc, ctx, &func, &inst);
	cli_event_time_stop(interp_ev, BCEV_EXEC_TIME);

	cli_event_int(interp_ev, BCEV_EXEC_RETURNVALUE, ret);
	cli_event_string(interp_ev, BCEV_VIRUSNAME, ctx->virname);

	/* need to be called here to catch any extracted but not yet scanned files
	*/
	if (ctx->outfd)
	    cli_bcapi_extract_new(ctx, -1);
    }
    if (bc->state == bc_jit || test_mode) {
	if (test_mode) {
	    ctx->off = 0;
	}
	ctx->bc_events = jit_ev;
	cli_dbgmsg("Bytecode %u: executing in JIT mode\n", bc->id);

	ctx->on_jit = 1;
	cli_event_time_start(jit_ev, BCEV_EXEC_TIME);
	ret = cli_vm_execute_jit(bcs, ctx, &bc->funcs[ctx->funcid]);
	cli_event_time_stop(jit_ev, BCEV_EXEC_TIME);

	cli_event_int(jit_ev, BCEV_EXEC_RETURNVALUE, ret);
	cli_event_string(jit_ev, BCEV_VIRUSNAME, ctx->virname);

	/* need to be called here to catch any extracted but not yet scanned files
	*/
	if (ctx->outfd)
	    cli_bcapi_extract_new(ctx, -1);
    }

    if (test_mode) {
	unsigned interp_errors = cli_event_errors(interp_ev);
	unsigned jit_errors = cli_event_errors(jit_ev);
	unsigned interp_warns = 0, jit_warns = 0;
	int ok = 1;
	enum bc_events evid;

	if (interp_errors || jit_errors) {
	    cli_infomsg(cctx, "bytecode %d encountered %u JIT and %u interpreter errors\n",
			bc->id, interp_errors, jit_errors);
	    ok = 0;
	}
	if (!ctx->no_diff && cli_event_diff_all(interp_ev, jit_ev, NULL)) {
	    cli_infomsg(cctx, "bytecode %d execution different with JIT and interpreter, see --debug for details\n",
			bc->id);
	    ok = 0;
	}
	for (evid=BCEV_API_WARN_BEGIN+1;evid < BCEV_API_WARN_END;evid++) {
	    union ev_val v;
	    uint32_t count = 0;
	    cli_event_get(interp_ev, evid, &v, &count);
	    interp_warns += count;
	    count = 0;
	    cli_event_get(jit_ev, evid, &v, &count);
	    jit_warns += count;
	}
	if (interp_warns || jit_warns) {
	    cli_infomsg(cctx, "bytecode %d encountered %u JIT and %u interpreter warnings\n",
			bc->id, interp_warns, jit_warns);
	    ok = 0;
	}
	/*cli_event_debug(jit_ev, BCEV_EXEC_TIME);
	cli_event_debug(interp_ev, BCEV_EXEC_TIME);*/
	if (!ok) {
	    cli_events_free(jit_ev);
	    cli_events_free(interp_ev);
	    return CL_EBYTECODE_TESTFAIL;
	}
    }
    cli_events_free(jit_ev);
    cli_events_free(interp_ev);
    if (cctx)
	cli_event_time_stop(cctx->perf, PERFT_BYTECODE);
    return ret;
}