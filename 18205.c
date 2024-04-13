static inline ut64 dwarf_read_address(size_t size, bool big_endian, const ut8 **buf, const ut8 *buf_end) {
	ut64 result;
	switch (size) {
	case 2:
		result = READ16(*buf);
		break;
	case 4:
		result = READ32(*buf);
		break;
	case 8:
		result = READ64(*buf);
		break;
	default:
		result = 0;
		*buf += size;
		eprintf("Weird dwarf address size: %zu.", size);
	}
	return result;
}