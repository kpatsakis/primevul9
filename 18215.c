RZ_API RzBinDwarfLineInfo *rz_bin_dwarf_parse_line(RzBinFile *binfile, RZ_NULLABLE RzBinDwarfDebugInfo *info, RzBinDwarfLineInfoMask mask) {
	rz_return_val_if_fail(binfile, NULL);
	RzBinSection *section = getsection(binfile, "debug_line");
	if (!section) {
		return NULL;
	}
	ut64 len = section->size;
	if (len < 1) {
		return NULL;
	}
	ut8 *buf = RZ_NEWS0(ut8, len + 1);
	if (!buf) {
		return NULL;
	}
	int ret = rz_buf_read_at(binfile->buf, section->paddr, buf, len);
	if (ret != len) {
		free(buf);
		return NULL;
	}
	// Actually parse the section
	RzBinDwarfLineInfo *r = parse_line_raw(binfile, buf, len, mask, binfile->o && binfile->o->info && binfile->o->info->big_endian, info);
	free(buf);
	return r;
}