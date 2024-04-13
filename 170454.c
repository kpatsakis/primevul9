void MACH0_(opts_set_default)(struct MACH0_(opts_t) * options, RzBinFile *bf) {
	rz_return_if_fail(options && bf && bf->rbin);
	options->header_at = 0;
	options->symbols_off = 0;
	options->verbose = bf->rbin->verbose;
	options->patch_relocs = true;
}