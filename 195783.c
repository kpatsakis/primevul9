static int add_selfcheck(struct cli_all_bc *bcs)
{
    struct cli_bc_func *func;
    struct cli_bc_inst *inst;
    struct cli_bc *bc;

    bcs->all_bcs = cli_realloc2(bcs->all_bcs, sizeof(*bcs->all_bcs)*(bcs->count+1));
    if (!bcs->all_bcs) {
	cli_errmsg("cli_loadcbc: Can't allocate memory for bytecode entry\n");
	return CL_EMEM;
    }
    bc = &bcs->all_bcs[bcs->count++];
    memset(bc, 0, sizeof(*bc));

    bc->trusted = 1;
    bc->num_globals = 1;
    bc->globals = cli_calloc(1, sizeof(*bc->globals));
    if (!bc->globals) {
	cli_errmsg("Failed to allocate memory for globals\n");
	return CL_EMEM;
    }
    bc->globals[0] = cli_calloc(1, sizeof(*bc->globals[0]));
    if (!bc->globals[0]) {
	cli_errmsg("Failed to allocate memory for globals\n");
	return CL_EMEM;
    }
    bc->globaltys = cli_calloc(1, sizeof(*bc->globaltys));
    if (!bc->globaltys) {
	cli_errmsg("Failed to allocate memory for globaltypes\n");
	return CL_EMEM;
    }
    bc->globaltys[0] = 32;
    *bc->globals[0] = 0;
    bc->id = ~0;
    bc->kind = 0;
    bc->num_types = 5;
    bc->num_func = 1;
    bc->funcs = cli_calloc(1, sizeof(*bc->funcs));
    if (!bc->funcs) {
	cli_errmsg("Failed to allocate memory for func\n");
	return CL_EMEM;
    }
    func = bc->funcs;
    func->numInsts = 2;
    func->numLocals = 1;
    func->numValues = 1;
    func->numConstants = 1;
    func->numBB = 1;
    func->returnType = 32;
    func->types = cli_calloc(1, sizeof(*func->types));
    if (!func->types) {
	cli_errmsg("Failed to allocate memory for types\n");
	return CL_EMEM;
    }
    func->types[0] = 32;
    func->BB = cli_calloc(1, sizeof(*func->BB));
    if (!func->BB) {
	cli_errmsg("Failed to allocate memory for BB\n");
	return CL_EMEM;
    }
    func->allinsts = cli_calloc(2, sizeof(*func->allinsts));
    if (!func->allinsts) {
	cli_errmsg("Failed to allocate memory for insts\n");
	return CL_EMEM;
    }
    func->BB->numInsts = 2;
    func->BB->insts = func->allinsts;
    func->constants = cli_calloc(1, sizeof(*func->constants));
    if (!func->constants) {
	cli_errmsg("Failed to allocate memory for constants\n");
	return CL_EMEM;
    }
    func->constants[0] = 0xf00d;
    inst = func->allinsts;

    inst->opcode = OP_BC_CALL_API;
    inst->u.ops.numOps = 1;
    inst->u.ops.opsizes = NULL;
    inst->u.ops.ops = cli_calloc(1, sizeof(*inst->u.ops.ops));
    if (!inst->u.ops.ops) {
	cli_errmsg("Failed to allocate memory for instructions\n");
	return CL_EMEM;
    }
    inst->u.ops.ops[0] = 1;
    inst->u.ops.funcid = 18; /* test2 */
    inst->dest = 0;
    inst->type = 32;
    inst->interp_op = inst->opcode* 5 + 3;

    inst = &func->allinsts[1];
    inst->opcode = OP_BC_RET;
    inst->type = 32;
    inst->u.unaryop = 0;
    inst->interp_op = inst->opcode* 5;

    bc->state = bc_loaded;
    return 0;
}