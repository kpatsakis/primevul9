void cli_bytecode_destroy(struct cli_bc *bc)
{
    unsigned i, j, k;
    free(bc->metadata.compiler);
    free(bc->metadata.sigmaker);

    if (bc->funcs) {
	for (i=0;i<bc->num_func;i++) {
	    struct cli_bc_func *f = &bc->funcs[i];
	    if (!f)
		continue;
	    free(f->types);

	    for (j=0;j<f->numBB;j++) {
		struct cli_bc_bb *BB = &f->BB[j];
		for(k=0;k<BB->numInsts;k++) {
		    struct cli_bc_inst *ii = &BB->insts[k];
		    if (operand_counts[ii->opcode] > 3 ||
			ii->opcode == OP_BC_CALL_DIRECT || ii->opcode == OP_BC_CALL_API) {
			free(ii->u.ops.ops);
			free(ii->u.ops.opsizes);
		    }
		}
	    }
	    free(f->BB);
	    free(f->allinsts);
	    free(f->constants);
	}
	free(bc->funcs);
    }
    if (bc->types) {
	for (i=NUM_STATIC_TYPES;i<bc->num_types;i++) {
	    if (bc->types[i].containedTypes)
		free(bc->types[i].containedTypes);
	}
	free(bc->types);
    }

    if (bc->globals) {
	for (i=0;i<bc->num_globals;i++) {
	    free(bc->globals[i]);
	}
	free(bc->globals);
    }
    if (bc->dbgnodes) {
	for (i=0;i<bc->dbgnode_cnt;i++) {
	    for (j=0;j<bc->dbgnodes[i].numelements;j++) {
		struct cli_bc_dbgnode_element *el =  &bc->dbgnodes[i].elements[j];
		if (el && el->string)
		    free(el->string);
	    }
	}
	free(bc->dbgnodes);
    }
    free(bc->globaltys);
    if (bc->uses_apis)
	cli_bitset_free(bc->uses_apis);
    free(bc->lsig);
    free(bc->globalBytes);
    memset(bc, 0, sizeof(*bc));
}