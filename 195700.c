static int16_t get_optype(const struct cli_bc_func *bcfunc, operand_t op)
{
    if (op >= bcfunc->numArgs + bcfunc->numLocals)
	return 0;
    return bcfunc->types[op]&0x7fff;
}