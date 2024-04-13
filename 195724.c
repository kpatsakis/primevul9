static int cli_bytecode_prepare_interpreter(struct cli_bc *bc)
{
    unsigned i, j, k;
    uint64_t *gmap;
    unsigned bcglobalid = cli_apicall_maxglobal - _FIRST_GLOBAL+2;
    bc->numGlobalBytes = 0;
    gmap = cli_malloc(bc->num_globals*sizeof(*gmap));
    if (!gmap)
	return CL_EMEM;
    for (j=0;j<bc->num_globals;j++) {
	uint16_t ty = bc->globaltys[j];
	unsigned align = typealign(bc, ty);
	assert(align);
	bc->numGlobalBytes  = (bc->numGlobalBytes + align-1)&(~(align-1));
	gmap[j] = bc->numGlobalBytes;
	bc->numGlobalBytes += typesize(bc, ty);
    }
    if (bc->numGlobalBytes) {
	bc->globalBytes = cli_calloc(1, bc->numGlobalBytes);
	if (!bc->globalBytes)
	    return CL_EMEM;
    } else
	bc->globalBytes = NULL;

    for (j=0;j<bc->num_globals;j++) {
	struct cli_bc_type *ty;
	if (bc->globaltys[j] < 65)
	    continue;
	ty = &bc->types[bc->globaltys[j]-65];
	switch (ty->kind) {
	    case DPointerType:
		{
		    uint64_t ptr;
		    if (bc->globals[j][1] >= _FIRST_GLOBAL) {
			ptr = ptr_compose(bc->globals[j][1] - _FIRST_GLOBAL + 1,
					    bc->globals[j][0]);
		    } else {
			if (bc->globals[j][1] > bc->num_globals)
			    continue;
			ptr = ptr_compose(bcglobalid,
					  gmap[bc->globals[j][1]] + bc->globals[j][0]);
		    }
		    *(uint64_t*)&bc->globalBytes[gmap[j]] = ptr;
		    break;
		}
	    case DArrayType:
		{
		    unsigned elsize, i, off = gmap[j];
		    /* TODO: support other than ints in arrays */
		    elsize = typesize(bc, ty->containedTypes[0]);
		    switch (elsize) {
			case 1:
			    for(i=0;i<ty->numElements;i++)
				bc->globalBytes[off+i] = bc->globals[j][i];
			    break;
			case 2:
			    for(i=0;i<ty->numElements;i++)
				*(uint16_t*)&bc->globalBytes[off+i*2] = bc->globals[j][i];
			    break;
			case 4:
			    for(i=0;i<ty->numElements;i++)
				*(uint32_t*)&bc->globalBytes[off+i*4] = bc->globals[j][i];
			    break;
			case 8:
			    for(i=0;i<ty->numElements;i++)
				*(uint64_t*)&bc->globalBytes[off+i*8] = bc->globals[j][i];
			    break;
			default:
			    cli_dbgmsg("interpreter: unsupported elsize: %u\n", elsize);
		    }
		    break;
		}
	    default:
		/*TODO*/
		if (!bc->globals[j][1])
		    continue; /* null */
		break;
	}
    }

    for (i=0;i<bc->num_func;i++) {
	struct cli_bc_func *bcfunc = &bc->funcs[i];
	unsigned totValues = bcfunc->numValues + bcfunc->numConstants + bc->num_globals;
	unsigned *map = cli_malloc(sizeof(*map)*totValues);
	if (!map)
	    return CL_EMEM;
	bcfunc->numBytes = 0;
	for (j=0;j<bcfunc->numValues;j++) {
	    uint16_t ty = bcfunc->types[j];
	    unsigned align;
	    align = typealign(bc, ty);
	    assert(!ty || typesize(bc, ty));
	    assert(align);
	    bcfunc->numBytes  = (bcfunc->numBytes + align-1)&(~(align-1));
	    map[j] = bcfunc->numBytes;
	    /* printf("%d -> %d, %u\n", j, map[j], typesize(bc, ty)); */
	    bcfunc->numBytes += typesize(bc, ty);
	    /* TODO: don't allow size 0, it is always a bug! */
	}
	bcfunc->numBytes = (bcfunc->numBytes + 7)&~7;
	for (j=0;j<bcfunc->numConstants;j++) {
	    map[bcfunc->numValues+j] = bcfunc->numBytes;
	    bcfunc->numBytes += 8;
	}
	for (j=0;j<bcfunc->numInsts;j++) {
	    struct cli_bc_inst *inst = &bcfunc->allinsts[j];
	    inst->dest = map[inst->dest];
	    switch (inst->opcode) {
		case OP_BC_ADD:
		case OP_BC_SUB:
		case OP_BC_MUL:
		case OP_BC_UDIV:
		case OP_BC_SDIV:
		case OP_BC_UREM:
		case OP_BC_SREM:
		case OP_BC_SHL:
		case OP_BC_LSHR:
		case OP_BC_ASHR:
		case OP_BC_AND:
		case OP_BC_OR:
		case OP_BC_XOR:
		case OP_BC_ICMP_EQ:
		case OP_BC_ICMP_NE:
		case OP_BC_ICMP_UGT:
		case OP_BC_ICMP_UGE:
		case OP_BC_ICMP_ULT:
		case OP_BC_ICMP_ULE:
		case OP_BC_ICMP_SGT:
		case OP_BC_ICMP_SGE:
		case OP_BC_ICMP_SLT:
		case OP_BC_ICMP_SLE:
		case OP_BC_COPY:
		case OP_BC_STORE:
		    MAP(inst->u.binop[0]);
		    MAP(inst->u.binop[1]);
		    break;
		case OP_BC_SEXT:
		case OP_BC_ZEXT:
		case OP_BC_TRUNC:
		    MAP(inst->u.cast.source);
		    break;
		case OP_BC_BRANCH:
		    MAP(inst->u.branch.condition);
		    break;
		case OP_BC_JMP:
		    break;
		case OP_BC_RET:
		    MAP(inst->u.unaryop);
		    break;
		case OP_BC_SELECT:
		    MAP(inst->u.three[0]);
		    MAP(inst->u.three[1]);
		    MAP(inst->u.three[2]);
		    break;
		case OP_BC_CALL_API:/* fall-through */
		case OP_BC_CALL_DIRECT:
		{
		    struct cli_bc_func *target = NULL;
		    if (inst->opcode == OP_BC_CALL_DIRECT) {
			target = &bc->funcs[inst->u.ops.funcid];
			if (inst->u.ops.funcid > bc->num_func) {
			    cli_errmsg("bytecode: called function out of range: %u > %u\n", inst->u.ops.funcid, bc->num_func);
			    return CL_EBYTECODE;
			}
			if (inst->u.ops.numOps != target->numArgs) {
			    cli_errmsg("bytecode: call operands don't match function prototype\n");
			    return CL_EBYTECODE;
			}
		    } else {
			/* APIs have at most 2 parameters always */
			if (inst->u.ops.numOps > 5) {
			    cli_errmsg("bytecode: call operands don't match function prototype\n");
			    return CL_EBYTECODE;
			}
		    }
		    if (inst->u.ops.numOps) {
			inst->u.ops.opsizes = cli_malloc(sizeof(*inst->u.ops.opsizes)*inst->u.ops.numOps);
			if (!inst->u.ops.opsizes) {
			    cli_errmsg("Out of memory when allocating operand sizes\n");
			    return CL_EMEM;
			}
		    } else
			inst->u.ops.opsizes = NULL;
		    for (k=0;k<inst->u.ops.numOps;k++) {
			MAPPTR(inst->u.ops.ops[k]);
			if (inst->opcode == OP_BC_CALL_DIRECT)
			    inst->u.ops.opsizes[k] = typesize(bc, target->types[k]);
			else
			    inst->u.ops.opsizes[k] = 32; /*XXX*/
		    }
		    break;
		}
		case OP_BC_LOAD:
		    MAPPTR(inst->u.unaryop);
		    break;
		case OP_BC_GEP1:
		    if (inst->u.three[1]&0x80000000 ||
			bcfunc->types[inst->u.binop[1]]&0x8000) {
                      cli_errmsg("bytecode: gep1 of alloca is not allowed\n");
                      return CL_EBYTECODE;
                    }
		    MAP(inst->u.three[1]);
		    MAP(inst->u.three[2]);
                    inst->u.three[0] = get_geptypesize(bc, inst->u.three[0]);
                    if (inst->u.three[0] == -1)
                      return CL_EBYTECODE;
                    break;
		case OP_BC_GEPZ:
		    /*three[0] is the type*/
		    if (inst->u.three[1]&0x80000000 ||
			bcfunc->types[inst->u.three[1]]&0x8000)
			inst->interp_op = 5*(inst->interp_op/5);
		    else
			inst->interp_op = 5*(inst->interp_op/5)+3;
		    MAP(inst->u.three[1]);
		    if (calc_gepz(bc, bcfunc, inst->u.three[0], inst->u.three[2]) == -1)
			return CL_EBYTECODE;
		    MAP(inst->u.three[2]);
		    break;
/*		case OP_BC_GEPN:
		    *TODO 
		    break;*/
		case OP_BC_MEMSET:
		case OP_BC_MEMCPY:
		case OP_BC_MEMMOVE:
		case OP_BC_MEMCMP:
		    MAPPTR(inst->u.three[0]);
		    MAPPTR(inst->u.three[1]);
		    MAP(inst->u.three[2]);
		    break;
		case OP_BC_RET_VOID:
		case OP_BC_ISBIGENDIAN:
		case OP_BC_ABORT:
		    /* no operands */
		    break;
		case OP_BC_BSWAP16:
		case OP_BC_BSWAP32:
		case OP_BC_BSWAP64:
		    MAP(inst->u.unaryop);
		    break;
		case OP_BC_PTRDIFF32:
		    MAPPTR(inst->u.binop[0]);
		    MAPPTR(inst->u.binop[1]);
		    break;
		case OP_BC_PTRTOINT64:
		    MAPPTR(inst->u.unaryop);
		    break;
		default:
		    cli_warnmsg("Bytecode: unhandled opcode: %d\n", inst->opcode);
		    return CL_EBYTECODE;
	    }
	}
	free(map);
    }
    free(gmap);
    bc->state = bc_interp;
    return CL_SUCCESS;
}