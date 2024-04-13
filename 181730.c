R_API int r_core_esil_step_back(RCore *core) {
	RAnalEsil *esil = core->anal->esil;
	RListIter *tail;
	const char *name = r_reg_get_name (core->anal->reg, R_REG_NAME_PC);
	ut64 prev = 0;
	ut64 end = r_reg_getv (core->anal->reg, name);

	if (!esil || !(tail = r_list_tail (esil->sessions))) {
		return 0;
	}
	RAnalEsilSession *before = (RAnalEsilSession *) tail->data;
	if (!before) {
		eprintf ("Cannot find any previous state here\n");
		return 0;
	}
	eprintf ("NOTE: step back in esil is setting an initial state and stepping into pc is the same.\n");
	eprintf ("NOTE: this is extremely wrong and poorly efficient. so don't use this feature unless\n");
	eprintf ("NOTE: you are going to fix it by making it consistent with dts, which is also broken as hell\n");
	eprintf ("Execute until 0x%08"PFMT64x"\n", end);
	r_anal_esil_session_set (esil, before);
	r_core_esil_step (core, end, NULL, &prev, false);
	eprintf ("Before 0x%08"PFMT64x"\n", prev);
	r_anal_esil_session_set (esil, before);
	r_core_esil_step (core, prev, NULL, NULL, false);
	return 1;
}