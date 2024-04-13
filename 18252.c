static inline ut64 dwarf_read_initial_length(RZ_OUT bool *is_64bit, bool big_endian, const ut8 **buf, const ut8 *buf_end) {
	ut64 r = READ32(*buf);
	if (r == DWARF_INIT_LEN_64) {
		r = READ64(*buf);
		*is_64bit = true;
	} else {
		*is_64bit = false;
	}
	return r;
}