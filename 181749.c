static void __updateStats(RCore *core, Sdb *db, ut64 addr, int statsMode) {
	RAnalOp *op = r_core_anal_op (core, addr, R_ANAL_OP_MASK_BASIC | R_ANAL_OP_MASK_HINT | R_ANAL_OP_MASK_DISASM);
	if (!op) {
		return;
	}
	if (statsMode == 'f') {
		const char *family = r_anal_op_family_to_string (op->family);
		sdb_num_inc (db, family, 1, 0);
	} else if (statsMode == 'o') {
		const char *type = r_anal_optype_to_string (op->type);
		sdb_num_inc (db, type, 1, 0);
	} else {
		char *mnem = strdup (op->mnemonic);
		char *sp = strchr (mnem, ' ');
		if (sp) {
			*sp = 0;
			//memmove (mnem, sp + 1, strlen (sp));
		}
		sdb_num_inc (db, mnem, 1, 0);
	}
	//sdb_set (db, family, "1", 0);
	//r_cons_printf ("0x%08"PFMT64x" %s\n", addr, family);
	r_anal_op_free (op);
	// r_core_cmdf (core, "pd 1 @ 0x%08"PFMT64x"\n", addr);
}