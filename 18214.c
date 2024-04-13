static RzBinDwarfDebugAbbrev *parse_abbrev_raw(const ut8 *obuf, size_t len) {
	const ut8 *buf = obuf, *buf_end = obuf + len;
	ut64 tmp, attr_code, attr_form, offset;
	st64 special;
	ut8 has_children;
	RzBinDwarfAbbrevDecl *tmpdecl;

	// XXX - Set a suitable value here.
	if (!obuf || len < 3) {
		return NULL;
	}
	RzBinDwarfDebugAbbrev *da = RZ_NEW0(RzBinDwarfDebugAbbrev);

	init_debug_abbrev(da);

	while (buf && buf + 1 < buf_end) {
		offset = buf - obuf;
		buf = rz_uleb128(buf, (size_t)(buf_end - buf), &tmp, NULL);
		if (!buf || !tmp || buf >= buf_end) {
			continue;
		}
		if (da->count == da->capacity) {
			expand_debug_abbrev(da);
		}
		tmpdecl = &da->decls[da->count];
		init_abbrev_decl(tmpdecl);

		tmpdecl->code = tmp;
		buf = rz_uleb128(buf, (size_t)(buf_end - buf), &tmp, NULL);
		tmpdecl->tag = tmp;

		tmpdecl->offset = offset;
		if (buf >= buf_end) {
			break;
		}
		has_children = READ8(buf);
		tmpdecl->has_children = has_children;
		do {
			if (tmpdecl->count == tmpdecl->capacity) {
				expand_abbrev_decl(tmpdecl);
			}
			buf = rz_uleb128(buf, (size_t)(buf_end - buf), &attr_code, NULL);
			if (buf >= buf_end) {
				break;
			}
			buf = rz_uleb128(buf, (size_t)(buf_end - buf), &attr_form, NULL);
			// http://www.dwarfstd.org/doc/DWARF5.pdf#page=225
			if (attr_form == DW_FORM_implicit_const) {
				buf = rz_leb128(buf, (size_t)(buf_end - buf), &special);
				tmpdecl->defs[tmpdecl->count].special = special;
			}
			tmpdecl->defs[tmpdecl->count].attr_name = attr_code;
			tmpdecl->defs[tmpdecl->count].attr_form = attr_form;
			tmpdecl->count++;
		} while (attr_code && attr_form);

		da->count++;
	}
	return da;
}