static int parse_function_starts(struct MACH0_(obj_t) * bin, ut64 off) {
	struct linkedit_data_command fc;
	ut8 sfc[sizeof(struct linkedit_data_command)] = { 0 };
	int len;

	if (off > bin->size || off + sizeof(struct linkedit_data_command) > bin->size) {
		bprintf("Likely overflow while parsing"
			" LC_FUNCTION_STARTS command\n");
	}
	bin->func_start = NULL;
	len = rz_buf_read_at(bin->b, off, sfc, sizeof(struct linkedit_data_command));
	if (len < 1) {
		bprintf("Failed to get data while parsing"
			" LC_FUNCTION_STARTS command\n");
	}
	fc.cmd = rz_read_ble32(&sfc[0], bin->big_endian);
	fc.cmdsize = rz_read_ble32(&sfc[4], bin->big_endian);
	fc.dataoff = rz_read_ble32(&sfc[8], bin->big_endian);
	fc.datasize = rz_read_ble32(&sfc[12], bin->big_endian);

	if ((int)fc.datasize > 0) {
		ut8 *buf = calloc(1, fc.datasize + 1);
		if (!buf) {
			bprintf("Failed to allocate buffer\n");
			return false;
		}
		bin->func_size = fc.datasize;
		if (fc.dataoff > bin->size || fc.dataoff + fc.datasize > bin->size) {
			free(buf);
			bprintf("Likely overflow while parsing "
				"LC_FUNCTION_STARTS command\n");
			return false;
		}
		len = rz_buf_read_at(bin->b, fc.dataoff, buf, fc.datasize);
		if (len != fc.datasize) {
			free(buf);
			bprintf("Failed to get data while parsing"
				" LC_FUNCTION_STARTS\n");
			return false;
		}
		buf[fc.datasize] = 0; // null-terminated buffer
		bin->func_start = buf;
		return true;
	}
	bin->func_start = NULL;
	return false;
}