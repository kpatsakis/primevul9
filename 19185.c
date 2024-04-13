static bool found_xref(RCore *core, ut64 at, ut64 xref_to, RAnalRefType type, PJ *pj, int rad, bool cfg_debug, bool cfg_anal_strings) {
	// Validate the reference. If virtual addressing is enabled, we
	// allow only references to virtual addresses in order to reduce
	// the number of false positives. In debugger mode, the reference
	// must point to a mapped memory region.
	if (type == R_ANAL_REF_TYPE_NULL) {
		return false;
	}
	if (cfg_debug) {
		if (!r_debug_map_get (core->dbg, xref_to)) {
			return false;
		}
	} else if (core->io->va) {
		if (!r_io_is_valid_offset (core->io, xref_to, 0)) {
			return false;
		}
	}
	if (!rad) {
		if (cfg_anal_strings && type == R_ANAL_REF_TYPE_DATA) {
			int len = 0;
			char *str_string = is_string_at (core, xref_to, &len);
			if (str_string) {
				r_name_filter (str_string, -1);
				char *str_flagname = r_str_newf ("str.%s", str_string);
				r_flag_space_push (core->flags, R_FLAGS_FS_STRINGS);
				(void)r_flag_set (core->flags, str_flagname, xref_to, 1);
				r_flag_space_pop (core->flags);
				free (str_flagname);
				if (len > 0) {
					r_meta_set (core->anal, R_META_TYPE_STRING, xref_to,
								len, (const char *) str_string);
				}
				free (str_string);
			}
		}
		// Add to SDB
		if (xref_to) {
			r_anal_xrefs_set (core->anal, at, xref_to, type);
		}
	} else if (rad == 'j') {
		r_strf_var (key, 32, "0x%"PFMT64x, xref_to);
		r_strf_var (value, 32, "0x%"PFMT64x, at);
		pj_ks (pj, key, value);
	} else {
		int len = 0;
		// Display in radare commands format
		char *cmd;
		switch (type) {
		case R_ANAL_REF_TYPE_CODE: cmd = "axc"; break;
		case R_ANAL_REF_TYPE_CALL: cmd = "axC"; break;
		case R_ANAL_REF_TYPE_DATA: cmd = "axd"; break;
		default: cmd = "ax"; break;
		}
		r_cons_printf ("%s 0x%08"PFMT64x" 0x%08"PFMT64x"\n", cmd, xref_to, at);
		if (cfg_anal_strings && type == R_ANAL_REF_TYPE_DATA) {
			char *str_flagname = is_string_at (core, xref_to, &len);
			if (str_flagname) {
				ut64 str_addr = xref_to;
				r_name_filter (str_flagname, -1);
				r_cons_printf ("f str.%s=0x%"PFMT64x"\n", str_flagname, str_addr);
				r_cons_printf ("Cs %d @ 0x%"PFMT64x"\n", len, str_addr);
				free (str_flagname);
			}
		}
	}
	return true;
}