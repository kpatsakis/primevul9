static void free_ht_comp_dir(HtUPKv *kv) {
	free(kv->value);
}