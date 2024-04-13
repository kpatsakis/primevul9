void MACH0_(kv_loadlibs)(struct MACH0_(obj_t) * bin) {
	int i;
	for (i = 0; i < bin->nlibs; i++) {
		sdb_set(bin->kv, sdb_fmt("libs.%d.name", i), bin->libs[i], 0);
	}
}