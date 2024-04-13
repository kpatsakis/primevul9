R_API RList *r_core_anal_fcn_get_calls(RCore *core, RAnalFunction *fcn) {
	RAnalRef *refi;
	RListIter *iter, *iter2;

	// get all references from this function
	RList *refs = r_anal_function_get_refs (fcn);
	// sanity check
	if (!r_list_empty (refs)) {
		// iterate over all the references and remove these which aren't of type call
		r_list_foreach_safe (refs, iter, iter2, refi) {
			if (refi->type != R_ANAL_REF_TYPE_CALL) {
				r_list_delete (refs, iter);
			}
		}
	}
	return refs;
}