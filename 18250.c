static inline ut64 get_max_offset(size_t addr_size) {
	switch (addr_size) {
	case 2:
		return UT16_MAX;
	case 4:
		return UT32_MAX;
	case 8:
		return UT64_MAX;
	}
	return 0;
}