RZ_API RzList /*<RzBinDwarfARangeSet>*/ *rz_bin_dwarf_parse_aranges(RzBinFile *binfile) {
	rz_return_val_if_fail(binfile, NULL);
	RzBinSection *section = getsection(binfile, "debug_aranges");
	if (!section) {
		return NULL;
	}
	size_t len = section->size;
	if (!len) {
		return NULL;
	}
	ut8 *buf = RZ_NEWS0(ut8, len);
	int ret = rz_buf_read_at(binfile->buf, section->paddr, buf, len);
	if (!ret) {
		free(buf);
		return NULL;
	}
	RzList *r = parse_aranges_raw(buf, len, binfile->o && binfile->o->info && binfile->o->info->big_endian);
	free(buf);
	return r;
}