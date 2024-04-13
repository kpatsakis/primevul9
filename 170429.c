static int parse_dylib(struct MACH0_(obj_t) * bin, ut64 off) {
	struct dylib_command dl;
	int lib, len;
	ut8 sdl[sizeof(struct dylib_command)] = { 0 };

	if (off > bin->size || off + sizeof(struct dylib_command) > bin->size) {
		return false;
	}
	lib = bin->nlibs - 1;

	void *relibs = realloc(bin->libs, bin->nlibs * RZ_BIN_MACH0_STRING_LENGTH);
	if (!relibs) {
		perror("realloc (libs)");
		return false;
	}
	bin->libs = relibs;
	len = rz_buf_read_at(bin->b, off, sdl, sizeof(struct dylib_command));
	if (len < 1) {
		bprintf("Error: read (dylib)\n");
		return false;
	}
	dl.cmd = rz_read_ble32(&sdl[0], bin->big_endian);
	dl.cmdsize = rz_read_ble32(&sdl[4], bin->big_endian);
	dl.dylib.name = rz_read_ble32(&sdl[8], bin->big_endian);
	dl.dylib.timestamp = rz_read_ble32(&sdl[12], bin->big_endian);
	dl.dylib.current_version = rz_read_ble32(&sdl[16], bin->big_endian);
	dl.dylib.compatibility_version = rz_read_ble32(&sdl[20], bin->big_endian);

	if (off + dl.dylib.name > bin->size ||
		off + dl.dylib.name + RZ_BIN_MACH0_STRING_LENGTH > bin->size) {
		return false;
	}

	memset(bin->libs[lib], 0, RZ_BIN_MACH0_STRING_LENGTH);
	len = rz_buf_read_at(bin->b, off + dl.dylib.name,
		(ut8 *)bin->libs[lib], RZ_BIN_MACH0_STRING_LENGTH);
	bin->libs[lib][RZ_BIN_MACH0_STRING_LENGTH - 1] = 0;
	if (len < 1) {
		bprintf("Error: read (dylib str)");
		return false;
	}
	return true;
}