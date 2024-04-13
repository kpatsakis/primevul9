static void free_loc_table_entry(HtUPKv *kv) {
	if (kv) {
		free_loc_table_list(kv->value);
	}
}