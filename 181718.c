static void list_vars(RCore *core, RAnalFunction *fcn, int type, const char *name) {
	RAnalVar *var;
	RListIter *iter;
	RList *list = r_anal_var_all_list (core->anal, fcn);
	if (type == '*') {
		const char *bp = r_reg_get_name (core->anal->reg, R_REG_NAME_BP);
		r_cons_printf ("f-fcnvar*\n");
		r_list_foreach (list, iter, var) {
			r_cons_printf ("f fcnvar.%s @ %s%s%d\n", var->name, bp,
				var->delta>=0? "+":"", var->delta);
		}
		return;
	}
	if (type != 'W' && type != 'R') {
		return;
	}
	const char *typestr = type == 'R'?"reads":"writes";
	if (name && *name) {
		var = r_anal_var_get_byname (core->anal, fcn->addr, name);
		if (var) {
			r_cons_printf ("%10s  ", var->name);
			var_accesses_list (core->anal, fcn, var->delta, typestr);
		}
	} else {
		r_list_foreach (list, iter, var) {
			r_cons_printf ("%10s  ", var->name);
			var_accesses_list (core->anal, fcn, var->delta, typestr);
		}
	}
}