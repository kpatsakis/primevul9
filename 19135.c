R_API char *r_core_anal_fcn_autoname(RCore *core, ut64 addr, int dump, int mode) {
	RAnalFunction *fcn = r_anal_get_fcn_in (core->anal, addr, 0);
	return fcn? anal_fcn_autoname (core, fcn, dump, mode): NULL;
}