static const ut8 *parse_comp_unit(RzBinDwarfDebugInfo *info, const ut8 *buf_start,
	RzBinDwarfCompUnit *unit, const RzBinDwarfDebugAbbrev *abbrevs,
	size_t first_abbr_idx, const ut8 *debug_str, size_t debug_str_len, bool big_endian) {

	const ut8 *buf = buf_start;
	const ut8 *buf_end = buf_start + unit->hdr.length - unit->hdr.header_size;

	while (buf && buf < buf_end && buf >= buf_start) {
		if (unit->count && unit->capacity == unit->count) {
			expand_cu(unit);
		}
		RzBinDwarfDie *die = &unit->dies[unit->count];
		// add header size to the offset;
		die->offset = buf - buf_start + unit->hdr.header_size + unit->offset;
		die->offset += unit->hdr.is_64bit ? 12 : 4;

		// DIE starts with ULEB128 with the abbreviation code
		ut64 abbr_code;
		buf = rz_uleb128(buf, buf_end - buf, &abbr_code, NULL);

		if (abbr_code > abbrevs->count || !buf) { // something invalid
			return NULL;
		}

		if (buf >= buf_end) {
			unit->count++; // we wanna store this entry too, usually the last one is null_entry
			return buf; // return the buffer to parse next compilation units
		}
		// there can be "null" entries that have abbr_code == 0
		if (!abbr_code) {
			unit->count++;
			continue;
		}
		ut64 abbr_idx = first_abbr_idx + abbr_code;

		if (abbrevs->count < abbr_idx) {
			return NULL;
		}
		RzBinDwarfAbbrevDecl *abbrev = &abbrevs->decls[abbr_idx - 1];

		if (init_die(die, abbr_code, abbrev->count)) {
			return NULL; // error
		}
		die->tag = abbrev->tag;
		die->has_children = abbrev->has_children;

		buf = parse_die(buf, buf_end, info, abbrev, &unit->hdr, die, debug_str, debug_str_len, big_endian);
		if (!buf) {
			return NULL;
		}

		unit->count++;
	}
	return buf;
}