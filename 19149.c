static RList *anal_graph_to(RCore *core, ut64 addr, int depth, HtUP *avoid) {
	RAnalFunction *cur_fcn = r_anal_get_fcn_in (core->anal, core->offset, 0);
	RList *list = r_list_new ();
	HtUP *state = ht_up_new0 ();

	if (!list || !state || !cur_fcn) {
		r_list_free (list);
		ht_up_free (state);
		return NULL;
	}

	// forward search
	if (anal_path_exists (core, core->offset, addr, list, depth - 1, state, avoid)) {
		ht_up_free (state);
		return list;
	}

	// backward search
	RList *xrefs = r_anal_xrefs_get (core->anal, cur_fcn->addr);
	if (xrefs) {
		RListIter *iter;
		RAnalRef *xref = NULL;
		r_list_foreach (xrefs, iter, xref) {
			if (xref->type == R_ANAL_REF_TYPE_CALL) {
				ut64 offset = core->offset;
				core->offset = xref->addr;
				r_list_free (list);
				list = anal_graph_to (core, addr, depth - 1, avoid);
				core->offset = offset;
				if (list && r_list_length (list)) {
					r_list_free (xrefs);
					ht_up_free (state);
					return list;
				}
			}
		}
	}

	r_list_free (xrefs);
	ht_up_free (state);
	r_list_free (list);
	return NULL;
}