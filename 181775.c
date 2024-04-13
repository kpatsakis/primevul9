static void showregs (RList *list) {
	if (!r_list_empty (list)) {
		char *reg;
		RListIter *iter;
		r_list_foreach (list, iter, reg) {
			r_cons_print (reg);
			if (iter->n) {
				r_cons_printf (" ");
			}
		}
	}
	r_cons_newline();
}