static void _CbInRangeAav(RCore *core, ut64 from, ut64 to, int vsize, int count, void *user) {
	bool asterisk = user != NULL;
	int arch_align = r_anal_archinfo (core->anal, R_ANAL_ARCHINFO_ALIGN);
	bool vinfun = r_config_get_i (core->config, "anal.vinfun");
	int searchAlign = r_config_get_i (core->config, "search.align");
	int align = (searchAlign > 0)? searchAlign: arch_align;
	if (align > 1) {
		if ((from % align) || (to % align)) {
			bool itsFine = false;
			if (archIsThumbable (core)) {
				if ((from & 1) || (to & 1)) {
					itsFine = true;
				}
			}
			if (!itsFine) {
				return;
			}
			if (core->anal->verbose) {
				eprintf ("Warning: aav: false positive in 0x%08"PFMT64x"\n", from);
			}
		}
	}
	if (!vinfun) {
		RAnalFunction *fcn = r_anal_get_fcn_in (core->anal, from, -1);
		if (fcn) {
			return;
		}
	}
	if (asterisk) {
		r_cons_printf ("ax 0x%"PFMT64x " 0x%"PFMT64x "\n", to, from);
		r_cons_printf ("Cd %d @ 0x%"PFMT64x "\n", vsize, from);
		r_cons_printf ("f+ aav.0x%08"PFMT64x "= 0x%08"PFMT64x, to, to);
	} else {
		r_anal_xrefs_set (core->anal, from, to, R_ANAL_REF_TYPE_NULL);
		// r_meta_add (core->anal, 'd', from, from + vsize, NULL);
		r_core_cmdf (core, "Cd %d @ 0x%"PFMT64x "\n", vsize, from);
		if (!r_flag_get_at (core->flags, to, false)) {
			char *name = r_str_newf ("aav.0x%08"PFMT64x, to);
			r_flag_set (core->flags, name, to, vsize);
			free (name);
		}
	}
}