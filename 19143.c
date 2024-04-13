static int r_anal_analyze_fcn_refs(RCore *core, RAnalFunction *fcn, int depth) {
	RListIter *iter;
	RAnalRef *ref;
	RList *refs = r_anal_function_get_refs (fcn);

	r_list_foreach (refs, iter, ref) {
		if (ref->addr == UT64_MAX) {
			continue;
		}
		switch (ref->type) {
		case R_ANAL_REF_TYPE_DATA:
			if (core->anal->opt.followdatarefs) {
				r_anal_try_get_fcn (core, ref, depth, 2);
			}
			break;
		case R_ANAL_REF_TYPE_CODE:
		case R_ANAL_REF_TYPE_CALL:
			r_core_anal_fcn (core, ref->addr, ref->at, ref->type, depth - 1);
			break;
		default:
			break;
		}
		// TODO: fix memleak here, fcn not freed even though it is
		// added in core->anal->fcns which is freed in r_anal_free()
	}
	r_list_free (refs);
	return 1;
}