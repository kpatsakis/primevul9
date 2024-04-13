static void showmem (RList *list) {
	if (!r_list_empty (list)) {
		AeaMemItem *item;
		RListIter *iter;
		r_list_foreach (list, iter, item) {
			r_cons_printf (" 0x%08"PFMT64x, item->addr);

		}
	}
	r_cons_newline ();
}