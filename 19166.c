R_API void r_core_anal_coderefs(RCore *core, ut64 addr) {
	RAnalFunction *fcn = r_anal_get_fcn_in (core->anal, addr, -1);
	if (fcn) {
		const char *me = fcn->name;
		RListIter *iter;
		RAnalRef *ref;
		RList *refs = r_anal_function_get_refs (fcn);
		r_cons_printf ("agn %s\n", me);
		r_list_foreach (refs, iter, ref) {
			r_strf_buffer (32);
			RFlagItem *item = r_flag_get_i (core->flags, ref->addr);
			const char *dst = item? item->name: r_strf ("0x%08"PFMT64x, ref->addr);
			r_cons_printf ("agn %s\n", dst);
			r_cons_printf ("age %s %s\n", me, dst);
		}
		r_list_free (refs);
	} else {
		eprintf("Not in a function. Use 'df' to define it.\n");
	}
}