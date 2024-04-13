static int parseTypes(struct cli_bc *bc, unsigned char *buffer)
{
    unsigned i, offset = 1, len = strlen((const char*)buffer);
    char ok=1;

    if (buffer[0] != 'T') {
	cli_errmsg("Invalid function types header: %c\n", buffer[0]);
	return CL_EMALFDB;
    }
    bc->start_tid = readFixedNumber(buffer, &offset, len, &ok, 2);
    if (bc->start_tid != BC_START_TID) {
	cli_warnmsg("Type start id mismatch: %u != %u\n", bc->start_tid,
		    BC_START_TID);
	return CL_BREAK;
    }
    add_static_types(bc);
    for (i=(BC_START_TID - 65);i<bc->num_types-1;i++) {
	struct cli_bc_type *ty = &bc->types[i];
	uint8_t t = readFixedNumber(buffer, &offset, len, &ok, 1);
	if (!ok) {
	    cli_errmsg("Error reading type kind\n");
	    return CL_EMALFDB;
	}
	switch (t) {
	    case 1:
		ty->kind = DFunctionType;
		ty->size = ty->align = sizeof(void*);
		parseType(bc, ty, buffer, &offset, len, &ok);
		if (!ok) {
		    cli_errmsg("Error parsing type %u\n", i);
		    return CL_EMALFDB;
		}
		if (!ty->numElements) {
		    cli_errmsg("Function with no return type? %u\n", i);
		    return CL_EMALFDB;
		}
		break;
	    case 2:
	    case 3:
		ty->kind = (t == 2) ? DPackedStructType : DStructType;
		ty->size = ty->align = 0;/* TODO:calculate size/align of structs */
		ty->align = 8;
		parseType(bc, ty, buffer, &offset, len, &ok);
		if (!ok) {
		    cli_errmsg("Error parsing type %u\n", i);
		    return CL_EMALFDB;
		}
		break;
	    case 4:
		ty->kind = DArrayType;
		/* number of elements of array, not subtypes! */
		ty->numElements = readNumber(buffer, &offset, len, &ok);
		if (!ok) {
		    cli_errmsg("Error parsing type %u\n", i);
		    return CL_EMALFDB;
		}
		/* fall-through */
	    case 5:
		if (t == 5) {
		    ty->kind = DPointerType;
		    ty->numElements = 1;
		}
		ty->containedTypes = cli_malloc(sizeof(*ty->containedTypes));
		if (!ty->containedTypes) {
		    cli_errmsg("Out of memory allocating containedType\n");
		    return CL_EMALFDB;
		}
		ty->containedTypes[0] = readTypeID(bc, buffer, &offset, len, &ok);
		if (!ok) {
		    cli_errmsg("Error parsing type %u\n", i);
		    return CL_EMALFDB;
		}
		if (t == 5) {
		    /* for interpreter, pointers 64-bit there */
		    ty->size = ty->align = 8;
		} else {
		    ty->size = ty->numElements*typesize(bc, ty->containedTypes[0]);
		    ty->align = typealign(bc, ty->containedTypes[0]);
		}
		break;
	    default:
		cli_errmsg("Invalid type kind: %u\n", t);
		return CL_EMALFDB;
	}
    }
    for (i=(BC_START_TID - 65);i<bc->num_types-1;i++) {
	struct cli_bc_type *ty = &bc->types[i];
	if (ty->kind == DArrayType) {
	    ty->size = ty->numElements*typesize(bc, ty->containedTypes[0]);
	    ty->align = typealign(bc, ty->containedTypes[0]);
	}
    }
    return CL_SUCCESS;
}