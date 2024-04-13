static int inSymtab(HtPP *hash, const char *name, ut64 addr) {
	bool found = false;
	char *key = rz_str_newf("%" PFMT64x ".%s", addr, name);
	ht_pp_find(hash, key, &found);
	if (found) {
		free(key);
		return true;
	}
	ht_pp_insert(hash, key, "1");
	free(key);
	return false;
}