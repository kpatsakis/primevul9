const char *MACH0_(get_os)(struct MACH0_(obj_t) * bin) {
	if (bin) {
		switch (bin->os) {
		case 1: return "macos";
		case 2: return "ios";
		case 3: return "watchos";
		case 4: return "tvos";
		}
	}
	return "darwin";
}