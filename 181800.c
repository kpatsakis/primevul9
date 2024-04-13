static void cmd_anal_ucall_ref (RCore *core, ut64 addr) {
	RAnalFunction * fcn = r_anal_get_function_at (core->anal, addr);
	if (fcn) {
		r_cons_printf (" ; %s", fcn->name);
	} else {
		r_cons_printf (" ; 0x%" PFMT64x, addr);
	}
}