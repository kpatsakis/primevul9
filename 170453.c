const char *MACH0_(get_intrp)(struct MACH0_(obj_t) * bin) {
	return bin ? bin->intrp : NULL;
}