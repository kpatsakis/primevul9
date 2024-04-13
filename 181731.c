static void esil_init (RCore *core) {
	const char *pc = r_reg_get_name (core->anal->reg, R_REG_NAME_PC);
	int noNULL = r_config_get_i (core->config, "esil.noNULL");
	opc = r_reg_getv (core->anal->reg, pc);
	if (!opc || opc==UT64_MAX) {
		opc = core->offset;
	}
	if (!core->anal->esil) {
		int iotrap = r_config_get_i (core->config, "esil.iotrap");
		ut64 stackSize = r_config_get_i (core->config, "esil.stack.size");
		unsigned int addrsize = r_config_get_i (core->config, "esil.addr.size");
		if (!(core->anal->esil = r_anal_esil_new (stackSize, iotrap, addrsize))) {
			R_FREE (regstate);
			return;
		}
		r_anal_esil_setup (core->anal->esil, core->anal, 0, 0, noNULL);
	}
	free (regstate);
	regstate = r_reg_arena_peek (core->anal->reg);
}