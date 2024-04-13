static void r_anal_set_stringrefs(RCore *core, RAnalFunction *fcn) {
	RListIter *iter;
	RAnalRef *ref;
	RList *refs = r_anal_function_get_refs (fcn);
	r_list_foreach (refs, iter, ref) {
		if (ref->type == R_ANAL_REF_TYPE_DATA &&
			r_bin_is_string (core->bin, ref->addr)) {
			r_anal_xrefs_set (core->anal, ref->at, ref->addr, R_ANAL_REF_TYPE_STRING);
		}
	}
	r_list_free (refs);
}