static uint16_t type_components(struct cli_bc *bc, uint16_t id, char *ok)
{
    unsigned i, sum=0;
    const struct cli_bc_type *ty;
    if (id <= 64)
	return 1;
    ty = &bc->types[id-65];
    /* TODO: protect against recursive types */
    switch (ty->kind) {
	case DFunctionType:
	    cli_errmsg("bytecode: function type not accepted for constant: %u\n", id);
	    /* don't accept functions as constant initializers */
	    *ok = 0;
	    return 0;
	case DPointerType:
	    return 2;
	case DStructType:
	case DPackedStructType:
	    for (i=0;i<ty->numElements;i++) {
		sum += type_components(bc, ty->containedTypes[i], ok);
	    }
	    return sum;
	case DArrayType:
	    return type_components(bc, ty->containedTypes[0], ok)*ty->numElements;
	default:
	    *ok = 0;
	    return 0;
    }
}