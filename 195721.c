static int calc_gepz(struct cli_bc *bc, struct cli_bc_func *func, uint16_t tid, operand_t op)
{
    unsigned off = 0, i;
    uint32_t *gepoff;
    const struct cli_bc_type *ty;
    if (tid >= bc->num_types + 65) {
	cli_errmsg("bytecode: typeid out of range %u >= %u\n", tid, bc->num_types);
	return -1;
    }
    if (tid <= 65) {
	cli_errmsg("bytecode: invalid type for gep (%u)\n", tid);
	return -1;
    }
    ty = &bc->types[tid - 65];
    if (ty->kind != DPointerType || ty->containedTypes[0] < 65) {
	cli_errmsg("bytecode: invalid gep type, must be pointer to nonint: %u\n", tid);
	return -1;
    }
    ty = &bc->types[ty->containedTypes[0] - 65];
    if (ty->kind != DStructType && ty->kind != DPackedStructType)
	return 0;
    gepoff = (uint32_t*)&func->constants[op - func->numValues];
    if (*gepoff >= ty->numElements) {
	cli_errmsg("bytecode: gep offset out of range: %d >= %d\n",(uint32_t)*gepoff, ty->numElements);
	return -1;
    }
    for (i=0;i<*gepoff;i++) {
	off += typesize(bc, ty->containedTypes[i]);
    }
    *gepoff = off;
    return 1;
}