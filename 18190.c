static inline ut64 dwarf_read_offset(bool is_64bit, bool big_endian, const ut8 **buf, const ut8 *buf_end) {
	ut64 result;
	if (is_64bit) {
		result = READ64(*buf);
	} else {
		result = READ32(*buf);
	}
	return result;
}