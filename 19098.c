R_API void r_core_anal_inflags(RCore *core, const char *glob) {
	RList *addrs = r_list_newf (free);
	RListIter *iter;
	bool a2f = r_config_get_i (core->config, "anal.a2f");
	char *anal_in = strdup (r_config_get (core->config, "anal.in"));
	r_config_set (core->config, "anal.in", "block");
	// aaFa = use a2f instead of af+
	bool simple = (!glob || *glob != 'a');
	glob = r_str_trim_head_ro (glob);
	char *addr;
	r_flag_foreach_glob (core->flags, glob, __cb, addrs);
	// should be sorted already
	r_list_sort (addrs, (RListComparator)__addrs_cmp);
	r_list_foreach (addrs, iter, addr) {
		if (!iter->n || r_cons_is_breaked ()) {
			break;
		}
		char *addr2 = iter->n->data;
		if (!addr || !addr2) {
			break;
		}
		ut64 a0 = r_num_get (NULL, addr);
		ut64 a1 = r_num_get (NULL, addr2);
		if (a0 == a1) {
			// ignore
			continue;
		}
		if (a0 > a1) {
			eprintf ("Warning: unsorted flag list 0x%"PFMT64x" 0x%"PFMT64x"\n", a0, a1);
			continue;
		}
		st64 sz = a1 - a0;
		if (sz < 1 || sz > core->anal->opt.bb_max_size) {
			eprintf ("Warning: invalid flag range from 0x%08"PFMT64x" to 0x%08"PFMT64x"\n", a0, a1);
			continue;
		}
		if (simple) {
			RFlagItem *fi = r_flag_get_at (core->flags, a0, 0);
			r_core_cmdf (core, "af+ %s fcn.%s", addr, fi? fi->name: addr);
			r_core_cmdf (core, "afb+ %s %s %d", addr, addr, (int)sz);
		} else {
			r_core_cmdf (core, "aab@%s!%s-%s\n", addr, addr2, addr);
			RAnalFunction *fcn = r_anal_get_fcn_in (core->anal, r_num_math (core->num, addr), 0);
			if (fcn) {
				eprintf ("%s  %s %"PFMT64d"    # %s\n", addr, "af", sz, fcn->name);
			} else {
				if (a2f) {
					r_core_cmdf (core, "a2f@%s!%s-%s\n", addr, addr2, addr);
				} else {
					r_core_cmdf (core, "af@%s!%s-%s\n", addr, addr2, addr);
				}
				fcn = r_anal_get_fcn_in (core->anal, r_num_math (core->num, addr), 0);
				eprintf ("%s  %s %.4"PFMT64d"   # %s\n", addr, "aab", sz, fcn?fcn->name: "");
			}
		}
	}
	r_list_free (addrs);
	r_config_set (core->config, "anal.in", anal_in);
	free (anal_in);
}