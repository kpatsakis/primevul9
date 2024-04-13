static unsigned typesize(const struct cli_bc *bc, uint16_t type)
{
    struct cli_bc_type *ty;
    unsigned j;

    type &= 0x7fff;
    if (!type)
	return 0;
    if (type <= 8)
	return 1;
    if (type <= 16)
	return 2;
    if (type <= 32)
	return 4;
    if (type <= 64)
	return 8;
    ty = &bc->types[type-65];
    if (ty->size)
	return ty->size;
    switch (ty->kind) {
	case 2:
	case 3:
	    for (j=0;j<ty->numElements;j++)
		ty->size += typesize(bc, ty->containedTypes[j]);
	    break;
	case 4:
	    ty->size = ty->numElements * typesize(bc, ty->containedTypes[0]);
	    break;
	default:
	    break;
    }
    if (!ty->size && ty->kind != DFunctionType) {
	cli_warnmsg("type %d size is 0\n", type-65);
    }
    return ty->size;
}