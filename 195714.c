int cli_bytecode_context_setfuncid(struct cli_bc_ctx *ctx, const struct cli_bc *bc, unsigned funcid)
{
    unsigned i, s=0;
    const struct cli_bc_func *func;
    if (funcid >= bc->num_func) {
	cli_errmsg("bytecode: function ID doesn't exist: %u\n", funcid);
	return CL_EARG;
    }
    func = ctx->func = &bc->funcs[funcid];
    ctx->bc = bc;
    ctx->numParams = func->numArgs;
    ctx->funcid = funcid;
    if (func->numArgs) {
	ctx->operands = cli_malloc(sizeof(*ctx->operands)*func->numArgs);
	if (!ctx->operands) {
	    cli_errmsg("bytecode: error allocating memory for parameters\n");
	    return CL_EMEM;
	}
	ctx->opsizes = cli_malloc(sizeof(*ctx->opsizes)*func->numArgs);
	if (!ctx->opsizes) {
	    cli_errmsg("bytecode: error allocating memory for opsizes\n");
	    return CL_EMEM;
	}
	for (i=0;i<func->numArgs;i++) {
	    unsigned al = typealign(bc, func->types[i]);
	    s = (s+al-1)&~(al-1);
	    ctx->operands[i] = s;
	    s += ctx->opsizes[i] = typesize(bc, func->types[i]);
	}
    }
    s += 8;/* return value */
    ctx->bytes = s;
    ctx->values = cli_malloc(s);
    if (!ctx->values) {
	cli_errmsg("bytecode: error allocating memory for parameters\n");
	return CL_EMEM;
    }
    return CL_SUCCESS;
}