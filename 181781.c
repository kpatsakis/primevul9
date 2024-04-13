static void showregs_json (RList *list, PJ *pj) {
	pj_a (pj);
	if (!r_list_empty (list)) {
		char *reg;
		RListIter *iter;

		r_list_foreach (list, iter, reg) {
			pj_s (pj, reg);
		}
	}
	pj_end (pj);
}