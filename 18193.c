RZ_API RzBinDwarfDebugInfo *rz_bin_dwarf_parse_info(RzBinFile *binfile, RzBinDwarfDebugAbbrev *da) {
	rz_return_val_if_fail(binfile && da, NULL);
	RzBinSection *section = getsection(binfile, "debug_info");
	if (!section) {
		return NULL;
	}

	RzBinDwarfDebugInfo *info = NULL;
	ut64 debug_str_len = 0;
	ut8 *debug_str_buf = NULL;

	RzBinSection *debug_str = debug_str = getsection(binfile, "debug_str");
	if (debug_str) {
		debug_str_len = debug_str->size;
		debug_str_buf = RZ_NEWS0(ut8, debug_str_len + 1);
		if (!debug_str_buf) {
			goto cave;
		}
		st64 ret = rz_buf_read_at(binfile->buf, debug_str->paddr,
			debug_str_buf, debug_str_len);
		if (!ret) {
			goto cave_debug_str_buf;
		}
	}

	ut64 len = section->size;
	if (!len) {
		goto cave_debug_str_buf;
	}
	ut8 *buf = RZ_NEWS0(ut8, len);
	if (!buf) {
		goto cave_debug_str_buf;
	}
	if (!rz_buf_read_at(binfile->buf, section->paddr, buf, len)) {
		goto cave_buf;
	}
	info = parse_info_raw(da, buf, len, debug_str_buf, debug_str_len,
		binfile->o && binfile->o->info && binfile->o->info->big_endian);
	if (!info) {
		goto cave_buf;
	}

	// build hashtable after whole parsing because of possible relocations
	if (info) {
		size_t i, j;
		for (i = 0; i < info->count; i++) {
			RzBinDwarfCompUnit *unit = &info->comp_units[i];
			for (j = 0; j < unit->count; j++) {
				RzBinDwarfDie *die = &unit->dies[j];
				ht_up_insert(info->lookup_table, die->offset, die); // optimization for further processing}
			}
		}
	}
cave_buf:
	free(buf);
cave_debug_str_buf:
	free(debug_str_buf);
cave:
	return info;
}