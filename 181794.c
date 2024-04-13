static void __cmd_afvf(RCore *core, const char *input) {
	RAnalFunction *fcn = r_anal_get_fcn_in (core->anal, core->offset, -1);
	RListIter *iter;
	RAnalVar *p;
	RList *list = r_anal_var_all_list (core->anal, fcn);
	r_list_sort (list, delta_cmp2);
	r_list_foreach (list, iter, p) {
		if (p->isarg || p->delta > 0) {
			continue;
		}
		const char *pad = r_str_pad (' ', 10 - strlen (p->name));
		r_cons_printf ("0x%08"PFMT64x"  %s:%s%s\n", (ut64)-p->delta, p->name, pad, p->type);
	}
	r_list_sort (list, delta_cmp);
	r_list_foreach (list, iter, p) {
		if (!p->isarg && p->delta < 0) {
			continue;
		}
		// TODO: only stack vars if (p->kind == 's') { }
		const char *pad = r_str_pad (' ', 10 - strlen (p->name));
		// XXX this 0x6a is a hack
		r_cons_printf ("0x%08"PFMT64x"  %s:%s%s\n", ((ut64)p->delta) - 0x6a, p->name, pad, p->type);
	}
	r_list_free (list);

}