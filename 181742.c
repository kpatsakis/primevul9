static void showmem_json (RList *list, PJ *pj) {
	pj_a (pj);
	if (!r_list_empty (list)) {
		RListIter *iter;
		AeaMemItem *item;
		r_list_foreach (list, iter, item) {
			pj_n (pj, item->addr);
		}
	}

	pj_end (pj);
}