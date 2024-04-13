static int parseBB(struct cli_bc *bc, unsigned func, unsigned bb, unsigned char *buffer)
{
    char ok=1;
    unsigned offset, len, i, last = 0;
    struct cli_bc_bb *BB;
    struct cli_bc_func *bcfunc = &bc->funcs[func];
    struct cli_bc_inst inst;

    if (bb >= bcfunc->numBB) {
	cli_errmsg("Found too many basic blocks\n");
	return CL_EMALFDB;
    }

    BB = &bcfunc->BB[bb];
    len = strlen((const char*) buffer);
    if (buffer[0] != 'B') {
	cli_errmsg("Invalid basic block header: %c\n", buffer[0]);
	return CL_EMALFDB;
    }
    offset = 1;
    BB->numInsts = 0;
    BB->insts = &bcfunc->allinsts[bcfunc->insn_idx];
    while (!last) {
	unsigned numOp;
	if (buffer[offset] == 'T') {
	    last = 1;
	    offset++;
	    /* terminators are void */
	    inst.type = 0;
	    inst.dest = 0;
	} else {
	    inst.type = readNumber(buffer, &offset, len, &ok);
	    inst.dest = readNumber(buffer, &offset, len, &ok);
	}
	inst.opcode = readFixedNumber(buffer, &offset, len, &ok, 2);
	if (!ok) {
	    cli_errmsg("Invalid type or operand\n");
	    return CL_EMALFDB;
	}
	if (inst.opcode >= OP_BC_INVALID) {
	    cli_errmsg("Invalid opcode: %u\n", inst.opcode);
	    return CL_EMALFDB;
	}

	switch (inst.opcode) {
	    case OP_BC_JMP:
		inst.u.jump = readBBID(bcfunc, buffer, &offset, len, &ok);
		break;
	    case OP_BC_RET:
		inst.type = readNumber(buffer, &offset, len, &ok);
		inst.u.unaryop = readOperand(bcfunc, buffer, &offset, len, &ok);
		break;
	    case OP_BC_BRANCH:
		inst.u.branch.condition = readOperand(bcfunc, buffer, &offset, len, &ok);
		inst.u.branch.br_true = readBBID(bcfunc, buffer, &offset, len, &ok);
		inst.u.branch.br_false = readBBID(bcfunc, buffer, &offset, len, &ok);
		break;
	    case OP_BC_CALL_API:/* fall-through */
	    case OP_BC_CALL_DIRECT:
		numOp = readFixedNumber(buffer, &offset, len, &ok, 1);
		if (ok) {
		    inst.u.ops.numOps = numOp;
		    inst.u.ops.opsizes=NULL;
		    if (!numOp) {
			inst.u.ops.ops = NULL;
		    } else {
			inst.u.ops.ops = cli_calloc(numOp, sizeof(*inst.u.ops.ops));
			if (!inst.u.ops.ops) {
			    cli_errmsg("Out of memory allocating operands\n");
			    return CL_EMEM;
			}
		    }
		    if (inst.opcode == OP_BC_CALL_DIRECT)
			inst.u.ops.funcid = readFuncID(bc, buffer, &offset, len, &ok);
		    else
			inst.u.ops.funcid = readAPIFuncID(bc, buffer, &offset, len, &ok);
		    for (i=0;i<numOp;i++) {
			inst.u.ops.ops[i] = readOperand(bcfunc, buffer, &offset, len, &ok);
		    }
		}
		break;
	    case OP_BC_ZEXT:
	    case OP_BC_SEXT:
	    case OP_BC_TRUNC:
		inst.u.cast.source = readOperand(bcfunc, buffer, &offset, len, &ok);
		inst.u.cast.mask = bcfunc->types[inst.u.cast.source];
		if (inst.u.cast.mask == 1)
		    inst.u.cast.size = 0;
		else if (inst.u.cast.mask <= 8)
		    inst.u.cast.size = 1;
		else if (inst.u.cast.mask <= 16)
		    inst.u.cast.size = 2;
		else if (inst.u.cast.mask <= 32)
		    inst.u.cast.size = 3;
		else if (inst.u.cast.mask <= 64)
		    inst.u.cast.size = 4;
		/* calculate mask */
		if (inst.opcode != OP_BC_SEXT)
		    inst.u.cast.mask = inst.u.cast.mask != 64 ?
			(1ull<<inst.u.cast.mask)-1 :
			~0ull;
		break;
	    case OP_BC_GEP1:
	    case OP_BC_GEPZ:
		inst.u.three[0] = readNumber(buffer, &offset, len, &ok);
		inst.u.three[1] = readOperand(bcfunc, buffer, &offset, len, &ok);
		inst.u.three[2] = readOperand(bcfunc, buffer, &offset, len, &ok);
		break;
	    case OP_BC_GEPN:
		numOp = readFixedNumber(buffer, &offset, len, &ok, 1);
		if (ok) {
		    inst.u.ops.numOps = numOp+2;
		    inst.u.ops.opsizes = NULL;
		    inst.u.ops.ops = cli_calloc(numOp+2, sizeof(*inst.u.ops.ops));
		    if (!inst.u.ops.ops) {
			cli_errmsg("Out of memory allocating operands\n");
			return CL_EMEM;
		    }
		    inst.u.ops.ops[0] = readNumber(buffer, &offset, len, &ok);
		    for (i=1;i<numOp+2;i++)
			inst.u.ops.ops[i] = readOperand(bcfunc, buffer, &offset, len, &ok);
		}
		break;
	    case OP_BC_ICMP_EQ:
	    case OP_BC_ICMP_NE:
	    case OP_BC_ICMP_UGT:
	    case OP_BC_ICMP_UGE:
	    case OP_BC_ICMP_ULT:
	    case OP_BC_ICMP_ULE:
	    case OP_BC_ICMP_SGT:
	    case OP_BC_ICMP_SGE:
	    case OP_BC_ICMP_SLE:
	    case OP_BC_ICMP_SLT:
		/* instruction type must be correct before readOperand! */
		inst.type = readNumber(buffer, &offset, len, &ok);
		/* fall-through */
	    default:
		numOp = operand_counts[inst.opcode];
		switch (numOp) {
		    case 0:
			break;
		    case 1:
			inst.u.unaryop = readOperand(bcfunc, buffer, &offset, len, &ok);
			break;
		    case 2:
			inst.u.binop[0] = readOperand(bcfunc, buffer, &offset, len, &ok);
			inst.u.binop[1] = readOperand(bcfunc, buffer, &offset, len, &ok);
			break;
		    case 3:
			inst.u.three[0] = readOperand(bcfunc, buffer, &offset, len, &ok);
			inst.u.three[1] = readOperand(bcfunc, buffer, &offset, len, &ok);
			inst.u.three[2] = readOperand(bcfunc, buffer, &offset, len, &ok);
			break;
		    default:
			cli_errmsg("Opcode %u with too many operands: %u?\n", inst.opcode, numOp);
			ok = 0;
			break;
		}
	}
	if (inst.opcode == OP_BC_STORE) {
	    int16_t t = get_optype(bcfunc, inst.u.binop[0]);
	    if (t)
		inst.type = t;
	}
	if (inst.opcode == OP_BC_COPY)
	    inst.type = get_optype(bcfunc, inst.u.binop[1]);
	if (!ok) {
	    cli_errmsg("Invalid instructions or operands\n");
	    return CL_EMALFDB;
	}
	if (bcfunc->insn_idx + BB->numInsts >= bcfunc->numInsts) {
	    cli_errmsg("More instructions than declared in total: %u > %u!\n",
		       bcfunc->insn_idx+BB->numInsts, bcfunc->numInsts);
	    return CL_EMALFDB;
	}
	inst.interp_op = inst.opcode*5;
	if (inst.type > 1) {
	    if (inst.type <= 8)
		inst.interp_op += 1;
	    else if (inst.type <= 16)
		inst.interp_op += 2;
	    else if (inst.type <= 32)
		inst.interp_op += 3;
	    else if (inst.type <= 65)
		inst.interp_op += 4;
	    else {
		cli_dbgmsg("unknown inst type: %d\n", inst.type);
	    }
	}
	BB->insts[BB->numInsts++] = inst;
    }
    if (bb+1 == bc->funcs[func].numBB) {
	if (buffer[offset] != 'E') {
	    cli_errmsg("Missing basicblock terminator, got: %c\n", buffer[offset]);
	    return CL_EMALFDB;
	}
	offset++;
    }
    if (buffer[offset] == 'D') {
	unsigned num;
	offset += 3;
	if (offset >= len)
	    return CL_EMALFDB;
	num = readNumber(buffer, &offset, len, &ok);
	if (!ok)
	    return CL_EMALFDB;
	if (num != bcfunc->numInsts) {
	    cli_errmsg("invalid number of dbg nodes, expected: %u, got: %u\n", bcfunc->numInsts, num);
	    return CL_EMALFDB;
	}
	bcfunc->dbgnodes = cli_malloc(num*sizeof(*bcfunc->dbgnodes));
	if (!bcfunc->dbgnodes)
	    return CL_EMEM;
	for (i=0;i<num;i++) {
	    bcfunc->dbgnodes[i] = readNumber(buffer, &offset, len, &ok);
	    if (!ok)
		return CL_EMALFDB;
	}
    }
    if (offset != len) {
	cli_errmsg("Trailing garbage in basicblock: %d extra bytes\n",
		   len-offset);
	return CL_EMALFDB;
    }
    bcfunc->numBytes = 0;
    bcfunc->insn_idx += BB->numInsts;
    return CL_SUCCESS;
}