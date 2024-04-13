int cli_bytecode_prepare2(struct cl_engine *engine, struct cli_all_bc *bcs, unsigned dconfmask)
{
    unsigned i, interp = 0, jitok = 0, jitcount=0;
    int rc;
    struct cli_bc_ctx *ctx;

    if (!bcs->count) {
	cli_dbgmsg("No bytecodes loaded, not running builtin test\n");
	return CL_SUCCESS;
    }

    cli_detect_environment(&bcs->env);
    switch (bcs->env.arch) {
	case arch_i386:
	case arch_x86_64:
	    if (!(dconfmask & BYTECODE_JIT_X86)) {
		cli_dbgmsg("Bytecode: disabled on X86 via DCONF\n");
		if (set_mode(engine, CL_BYTECODE_MODE_INTERPRETER) == -1)
		    return CL_EBYTECODE_TESTFAIL;
	    }
	    break;
	case arch_ppc32:
	case arch_ppc64:
	    if (!(dconfmask & BYTECODE_JIT_PPC)) {
		cli_dbgmsg("Bytecode: disabled on PPC via DCONF\n");
		if (set_mode(engine, CL_BYTECODE_MODE_INTERPRETER) == -1)
		    return CL_EBYTECODE_TESTFAIL;
	    }
	    break;
	case arch_arm:
	    if (!(dconfmask & BYTECODE_JIT_ARM)) {
		cli_dbgmsg("Bytecode: disabled on ARM via DCONF\n");
		if (set_mode(engine, CL_BYTECODE_MODE_INTERPRETER) == -1)
		    return CL_EBYTECODE_TESTFAIL;
	    }
	    break;
	default:
	    cli_dbgmsg("Bytecode: JIT not supported on this architecture, falling back\n");
	    if (set_mode(engine, CL_BYTECODE_MODE_INTERPRETER) == -1)
		return CL_EBYTECODE_TESTFAIL;
	    break;
    }
    cli_dbgmsg("Bytecode: mode is %d\n", engine->bytecode_mode);

    ctx = cli_bytecode_context_alloc();
    if (!ctx) {
	cli_errmsg("Bytecode: failed to allocate bytecode context\n");
	return CL_EMEM;
    }
    rc = run_builtin_or_loaded(bcs, BC_STARTUP, builtin_bc_startup, ctx, "BC_STARTUP");
    if (rc != CL_SUCCESS) {
	cli_warnmsg("Bytecode: BC_STARTUP failed to run, disabling ALL bytecodes! Please report to http://bugs.clamav.net\n");
	ctx->bytecode_disable_status = 2;
    } else {
	cli_dbgmsg("Bytecode: disable status is %d\n", ctx->bytecode_disable_status);
	rc = cli_bytecode_context_getresult_int(ctx);
	/* check magic number, don't use 0 here because it is too easy for a
	 * buggy bytecode to return 0 */
	if (rc != 0xda7aba5e) {
	    cli_warnmsg("Bytecode: selftest failed with code %08x. Please report to http://bugs.clamav.net\n",
			rc);
	    if (engine->bytecode_mode == CL_BYTECODE_MODE_TEST)
		return CL_EBYTECODE_TESTFAIL;
	}
    }
    switch (ctx->bytecode_disable_status) {
	case 1:
	    if (set_mode(engine, CL_BYTECODE_MODE_INTERPRETER) == -1)
		return CL_EBYTECODE_TESTFAIL;
	    break;
	case 2:
	    if (set_mode(engine, CL_BYTECODE_MODE_OFF) == -1)
		return CL_EBYTECODE_TESTFAIL;
	    break;
	default:
	    break;
    }
    cli_bytecode_context_destroy(ctx);


    if (engine->bytecode_mode != CL_BYTECODE_MODE_INTERPRETER &&
	engine->bytecode_mode != CL_BYTECODE_MODE_OFF) {
	selfcheck(1, bcs->engine);
	rc = cli_bytecode_prepare_jit(bcs);
	if (rc == CL_SUCCESS) {
	    jitok = 1;
	    cli_dbgmsg("Bytecode: %u bytecode prepared with JIT\n", bcs->count);
	    if (engine->bytecode_mode != CL_BYTECODE_MODE_TEST)
		return CL_SUCCESS;
	}
	if (engine->bytecode_mode == CL_BYTECODE_MODE_JIT) {
	    cli_errmsg("Bytecode: JIT required, but not all bytecodes could be prepared with JIT\n");
	    return CL_EMALFDB;
	}
	if (rc && engine->bytecode_mode == CL_BYTECODE_MODE_TEST) {
	    cli_errmsg("Bytecode: Test mode, but not all bytecodes could be prepared with JIT\n");
	    return CL_EBYTECODE_TESTFAIL;
	}
    } else {
	cli_bytecode_done_jit(bcs, 0);
    }

    if (!(dconfmask & BYTECODE_INTERPRETER)) {
	cli_dbgmsg("Bytecode: needs interpreter, but interpreter is disabled\n");
	if (set_mode(engine, CL_BYTECODE_MODE_OFF) == -1)
	    return CL_EBYTECODE_TESTFAIL;
    }

    if (engine->bytecode_mode == CL_BYTECODE_MODE_OFF) {
	for (i=0;i<bcs->count;i++)
	    bcs->all_bcs[i].state = bc_disabled;
	cli_dbgmsg("Bytecode: ALL bytecodes disabled\n");
	return CL_SUCCESS;
    }

    for (i=0;i<bcs->count;i++) {
	struct cli_bc *bc = &bcs->all_bcs[i];
	if (bc->state == bc_jit) {
	    jitcount++;
	    if (engine->bytecode_mode != CL_BYTECODE_MODE_TEST)
		continue;
	}
	if (bc->state == bc_interp) {
	    interp++;
	    continue;
	}
	rc = cli_bytecode_prepare_interpreter(bc);
	if (rc != CL_SUCCESS) {
	    bc->state = bc_disabled;
	    cli_warnmsg("Bytecode: %d failed to prepare for interpreter mode\n", bc->id);
	    return rc;
	}
	interp++;
    }
    cli_dbgmsg("Bytecode: %u bytecode prepared with JIT, "
	       "%u prepared with interpreter, %u total\n", jitcount, interp, bcs->count);
    return CL_SUCCESS;
}