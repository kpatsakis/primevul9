static RzBinDwarfDebugInfo *parse_info_raw(RzBinDwarfDebugAbbrev *da,
	const ut8 *obuf, size_t len,
	const ut8 *debug_str, size_t debug_str_len, bool big_endian) {

	rz_return_val_if_fail(da && obuf, false);

	const ut8 *buf = obuf;
	const ut8 *buf_end = obuf + len;

	RzBinDwarfDebugInfo *info = RZ_NEW0(RzBinDwarfDebugInfo);
	if (!info) {
		return NULL;
	}
	if (!init_debug_info(info)) {
		goto cleanup;
	}
	int unit_idx = 0;

	while (buf < buf_end) {
		if (info->count >= info->capacity) {
			if (expand_info(info)) {
				break;
			}
		}

		RzBinDwarfCompUnit *unit = &info->comp_units[unit_idx];
		if (init_comp_unit(unit) < 0) {
			unit_idx--;
			goto cleanup;
		}
		info->count++;

		unit->offset = buf - obuf;
		// small redundancy, because it was easiest solution at a time
		unit->hdr.unit_offset = buf - obuf;

		buf = info_comp_unit_read_hdr(buf, buf_end, &unit->hdr, big_endian);

		if (unit->hdr.length > len) {
			goto cleanup;
		}

		if (da->decls->count >= da->capacity) {
			eprintf("WARNING: malformed dwarf have not enough buckets for decls.\n");
		}
		rz_warn_if_fail(da->count <= da->capacity);

		// find abbrev start for current comp unit
		// we could also do naive, ((char *)da->decls) + abbrev_offset,
		// but this is more bulletproof to invalid DWARF
		RzBinDwarfAbbrevDecl key = { .offset = unit->hdr.abbrev_offset };
		RzBinDwarfAbbrevDecl *abbrev_start = bsearch(&key, da->decls, da->count, sizeof(key), abbrev_cmp);
		if (!abbrev_start) {
			goto cleanup;
		}
		// They point to the same array object, so should be def. behaviour
		size_t first_abbr_idx = abbrev_start - da->decls;

		buf = parse_comp_unit(info, buf, unit, da, first_abbr_idx, debug_str, debug_str_len, big_endian);

		if (!buf) {
			goto cleanup;
		}

		unit_idx++;
	}

	return info;

cleanup:
	rz_bin_dwarf_debug_info_free(info);
	return NULL;
}