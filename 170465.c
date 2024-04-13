struct lib_t *MACH0_(get_libs)(struct MACH0_(obj_t) * bin) {
	struct lib_t *libs;
	int i;

	if (!bin->nlibs) {
		return NULL;
	}
	if (!(libs = calloc((bin->nlibs + 1), sizeof(struct lib_t)))) {
		return NULL;
	}
	for (i = 0; i < bin->nlibs; i++) {
		sdb_set(bin->kv, sdb_fmt("libs.%d.name", i), bin->libs[i], 0);
		strncpy(libs[i].name, bin->libs[i], RZ_BIN_MACH0_STRING_LENGTH - 1);
		libs[i].name[RZ_BIN_MACH0_STRING_LENGTH - 1] = '\0';
		libs[i].last = 0;
	}
	libs[i].last = 1;
	return libs;
}