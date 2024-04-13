static void fcn_list_bbs(RAnalFunction *fcn) {
	RAnalBlock *bbi;
	RListIter *iter;

	r_list_foreach (fcn->bbs, iter, bbi) {
		r_cons_printf ("afb+ 0x%08" PFMT64x " 0x%08" PFMT64x " %" PFMT64u " ",
				   fcn->addr, bbi->addr, bbi->size);
		r_cons_printf ("0x%08"PFMT64x" ", bbi->jump);
		r_cons_printf ("0x%08"PFMT64x, bbi->fail);
		if (bbi->diff) {
			if (bbi->diff->type == R_ANAL_DIFF_TYPE_MATCH) {
				r_cons_printf (" m");
			} else if (bbi->diff->type == R_ANAL_DIFF_TYPE_UNMATCH) {
				r_cons_printf (" u");
			} else {
				r_cons_printf (" n");
			}
		}
		r_cons_printf ("\n");
	}
}