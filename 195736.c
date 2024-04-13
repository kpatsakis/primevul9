int cli_bytecode_context_setparam_int(struct cli_bc_ctx *ctx, unsigned i, uint64_t c)
{
    if (i >= ctx->numParams) {
	cli_errmsg("bytecode: param index out of bounds: %u\n", i);
	return CL_EARG;
    }
    if (!type_isint(ctx->func->types[i])) {
	cli_errmsg("bytecode: parameter type mismatch\n");
	return CL_EARG;
    }
    switch (ctx->opsizes[i]) {
	case 1:
	    ctx->values[ctx->operands[i]] = c;
	    break;
	case 2:
	    *(uint16_t*)&ctx->values[ctx->operands[i]] = c;
	    break;
	case 4:
	    *(uint32_t*)&ctx->values[ctx->operands[i]] = c;
	    break;
	case 8:
	    *(uint64_t*)&ctx->values[ctx->operands[i]] = c;
	    break;
    }
    return CL_SUCCESS;
}