static int fcnlist_gather_metadata(RAnal *anal, RList *fcns) {
	RListIter *iter;
	RAnalFunction *fcn;
	RList *xrefs;

	r_list_foreach (fcns, iter, fcn) {
		// Count the number of references and number of calls
		RListIter *callrefiter;
		RAnalRef *ref;
		RList *refs = r_anal_function_get_refs (fcn);
		int numcallrefs = 0;
		r_list_foreach (refs, callrefiter, ref) {
			if (ref->type == R_ANAL_REF_TYPE_CALL) {
				numcallrefs++;
			}
		}
		r_list_free (refs);
		fcn->meta.numcallrefs = numcallrefs;
		xrefs = r_anal_xrefs_get (anal, fcn->addr);
		fcn->meta.numrefs = xrefs? xrefs->length: 0;
		r_list_free (xrefs);
	}
	// TODO: Determine sgnc, sgec
	return 0;
}