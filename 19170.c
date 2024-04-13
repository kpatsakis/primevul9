static bool printAnalPaths(RCoreAnalPaths *p, PJ *pj) {
	RListIter *iter;
	RAnalBlock *path;
	if (pj) {
		pj_a (pj);
	} else {
		r_cons_printf ("pdb @@= ");
	}

	r_list_foreach (p->path, iter, path) {
		if (pj) {
			pj_n (pj, path->addr);
		} else {
			r_cons_printf ("0x%08"PFMT64x" ", path->addr);
		}
	}

	if(pj) {
		pj_end (pj);
	} else {
		r_cons_printf ("\n");
	}
	return (p->count < 1 || --p->count > 0);
}