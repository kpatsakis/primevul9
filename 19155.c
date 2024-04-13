static void add_string_ref(RCore *core, ut64 xref_from, ut64 xref_to) {
	int len = 0;
	if (xref_to == UT64_MAX || !xref_to) {
		return;
	}
	if (!xref_from || xref_from == UT64_MAX) {
		xref_from = core->anal->esil->address;
	}
	char *str_flagname = is_string_at (core, xref_to, &len);
	if (str_flagname) {
		r_anal_xrefs_set (core->anal, xref_from, xref_to, R_ANAL_REF_TYPE_DATA);
		r_name_filter (str_flagname, -1);
		r_flag_space_push (core->flags, R_FLAGS_FS_STRINGS);
		char *flagname = r_str_newf ("str.%s", str_flagname);
		r_flag_set (core->flags, flagname, xref_to, len);
		free (flagname);
		r_flag_space_pop (core->flags);
		r_meta_set (core->anal, 's', xref_to, len, str_flagname);
		free (str_flagname);
	}
}