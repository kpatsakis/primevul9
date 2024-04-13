static void getpcfromstack(RCore *core, RAnalEsil *esil) {
	ut64 cur;
	ut64 addr;
	ut64 size;
	int idx;
	RAnalEsil esil_cpy;
	RAnalOp op = R_EMPTY;
	RAnalFunction *fcn = NULL;
	ut8 *buf = NULL;
	char *tmp_esil_str = NULL;
	int tmp_esil_str_len;
	const char *esilstr;
	const int maxaddrlen = 20;
	const char *spname = NULL;
	if (!esil) {
		return;
	}

	memcpy (&esil_cpy, esil, sizeof (esil_cpy));
	addr = cur = esil_cpy.cur;
	fcn = r_anal_get_fcn_in (core->anal, addr, 0);
	if (!fcn) {
		return;
	}

	size = r_anal_function_linear_size (fcn);
	if (size <= 0) {
		return;
	}

	buf = malloc (size + 2);
	if (!buf) {
		perror ("malloc");
		return;
	}

	r_io_read_at (core->io, addr, buf, size + 1);

	// TODO Hardcoding for 2 instructions (mov e_p,[esp];ret). More work needed
	idx = 0;
	if (r_anal_op (core->anal, &op, cur, buf + idx, size - idx, R_ANAL_OP_MASK_ESIL) <= 0 ||
			op.size <= 0 ||
			(op.type != R_ANAL_OP_TYPE_MOV && op.type != R_ANAL_OP_TYPE_CMOV)) {
		goto err_anal_op;
	}

	r_asm_set_pc (core->rasm, cur);
	esilstr = R_STRBUF_SAFEGET (&op.esil);
	if (!esilstr) {
		goto err_anal_op;
	}
	// Ugly code
	// This is a hack, since ESIL doesn't always preserve values pushed on the stack. That probably needs to be rectified
	spname = r_reg_get_name (core->anal->reg, R_REG_NAME_SP);
	if (!spname || !*spname) {
		goto err_anal_op;
	}
	tmp_esil_str_len = strlen (esilstr) + strlen (spname) + maxaddrlen;
	tmp_esil_str = (char*) malloc (tmp_esil_str_len);
	if (!tmp_esil_str) {
		goto err_anal_op;
	}
	tmp_esil_str[tmp_esil_str_len - 1] = '\0';
	snprintf (tmp_esil_str, tmp_esil_str_len - 1, "%s,[", spname);
	if (!*esilstr || (strncmp ( esilstr, tmp_esil_str, strlen (tmp_esil_str)))) {
		free (tmp_esil_str);
		goto err_anal_op;
	}

	snprintf (tmp_esil_str, tmp_esil_str_len - 1, "%20" PFMT64u "%s", esil_cpy.old, &esilstr[strlen (spname) + 4]);
	r_str_trim (tmp_esil_str);
	idx += op.size;
	r_anal_esil_set_pc (&esil_cpy, cur);
	r_anal_esil_parse (&esil_cpy, tmp_esil_str);
	r_anal_esil_stack_free (&esil_cpy);
	free (tmp_esil_str);

	cur = addr + idx;
	r_anal_op_fini (&op);
	if (r_anal_op (core->anal, &op, cur, buf + idx, size - idx, R_ANAL_OP_MASK_ESIL) <= 0 ||
			op.size <= 0 ||
			(op.type != R_ANAL_OP_TYPE_RET && op.type != R_ANAL_OP_TYPE_CRET)) {
		goto err_anal_op;
	}
	r_asm_set_pc (core->rasm, cur);

	esilstr = R_STRBUF_SAFEGET (&op.esil);
	r_anal_esil_set_pc (&esil_cpy, cur);
	if (!esilstr || !*esilstr) {
		goto err_anal_op;
	}
	r_anal_esil_parse (&esil_cpy, esilstr);
	r_anal_esil_stack_free (&esil_cpy);

	memcpy (esil, &esil_cpy, sizeof (esil_cpy));

 err_anal_op:
	r_anal_op_fini (&op);
	free (buf);
}