char *MACH0_(get_class)(struct MACH0_(obj_t) * bin) {
#if RZ_BIN_MACH064
	return rz_str_new("MACH064");
#else
	return rz_str_new("MACH0");
#endif
}