static void cmd_anal_blocks(RCore *core, const char *input) {
	ut64 from , to;
	char *arg = strchr (input, ' ');
	r_cons_break_push (NULL, NULL);
	if (!arg) {
		RList *list = r_core_get_boundaries_prot (core, R_PERM_X, NULL, "anal");
		RListIter *iter;
		RIOMap* map;
		if (!list) {
			goto ctrl_c;
		}
		r_list_foreach (list, iter, map) {
			from = map->itv.addr;
			to = r_itv_end (map->itv);
			if (r_cons_is_breaked ()) {
				goto ctrl_c;
			}
			if (!from && !to) {
				eprintf ("Cannot determine search boundaries\n");
			} else if (to - from > UT32_MAX) {
				eprintf ("Skipping huge range\n");
			} else {
				r_core_cmdf (core, "abb 0x%08"PFMT64x" @ 0x%08"PFMT64x, (to - from), from);
			}
		}
	} else {
		st64 sz = r_num_math (core->num, arg + 1);
		if (sz < 1) {
			eprintf ("Invalid range\n");
			return;
		}
		r_core_cmdf (core, "abb 0x%08"PFMT64x" @ 0x%08"PFMT64x, sz, core->offset);
	}
ctrl_c:
	r_cons_break_pop ();
}