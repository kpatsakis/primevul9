char *MACH0_(get_filetype_from_hdr)(struct MACH0_(mach_header) * hdr) {
	const char *mhtype = "Unknown";
	switch (hdr->filetype) {
	case MH_OBJECT: mhtype = "Relocatable object"; break;
	case MH_EXECUTE: mhtype = "Executable file"; break;
	case MH_FVMLIB: mhtype = "Fixed VM shared library"; break;
	case MH_CORE: mhtype = "Core file"; break;
	case MH_PRELOAD: mhtype = "Preloaded executable file"; break;
	case MH_DYLIB: mhtype = "Dynamically bound shared library"; break;
	case MH_DYLINKER: mhtype = "Dynamic link editor"; break;
	case MH_BUNDLE: mhtype = "Dynamically bound bundle file"; break;
	case MH_DYLIB_STUB: mhtype = "Shared library stub for static linking (no sections)"; break;
	case MH_DSYM: mhtype = "Companion file with only debug sections"; break;
	case MH_KEXT_BUNDLE: mhtype = "Kernel extension bundle file"; break;
	case MH_FILESET: mhtype = "Kernel cache file"; break;
	}
	return strdup(mhtype);
}