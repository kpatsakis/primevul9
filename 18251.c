RZ_API RzBinDwarfDebugAbbrev *rz_bin_dwarf_parse_abbrev(RzBinFile *binfile) {
	rz_return_val_if_fail(binfile, NULL);
	size_t len = 0;
	ut8 *buf = get_section_bytes(binfile, "debug_abbrev", &len);
	if (!buf) {
		return NULL;
	}
	RzBinDwarfDebugAbbrev *abbrevs = parse_abbrev_raw(buf, len);
	free(buf);
	return abbrevs;
}