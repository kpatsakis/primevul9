R_API int r_core_anal_esil_fcn(RCore *core, ut64 at, ut64 from, int reftype, int depth) {
	const char *esil;
	eprintf ("TODO\n");
	while (1) {
		// TODO: Implement the proper logic for doing esil analysis
		RAnalOp *op = r_core_anal_op (core, at, R_ANAL_OP_MASK_ESIL);
		if (!op) {
			break;
		}
		esil = R_STRBUF_SAFEGET (&op->esil);
		eprintf ("0x%08"PFMT64x" %d %s\n", at, op->size, esil);
		// at += op->size;
		// esilIsRet()
		// esilIsCall()
		// esilIsJmp()
		r_anal_op_free (op);
		break;
	}
	return 0;
}