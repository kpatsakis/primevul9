static void free_loc_table_list(RzBinDwarfLocList *loc_list) {
	RzListIter *iter;
	RzBinDwarfLocRange *range;
	rz_list_foreach (loc_list->list, iter, range) {
		free(range->expression->data);
		free(range->expression);
		free(range);
	}
	rz_list_free(loc_list->list);
	free(loc_list);
}