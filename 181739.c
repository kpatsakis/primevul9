R_API int r_core_anal_refs(RCore *core, const char *input) {
	int cfg_debug = r_config_get_i (core->config, "cfg.debug");
	ut64 from, to;
	int rad;
	if (*input == '?') {
		r_core_cmd_help (core, help_msg_aar);
		return 0;
	}

	if (*input == 'j' || *input == '*') {
		rad = *input;
		input++;
	} else {
		rad = 0;
	}

	from = to = 0;
	char *ptr = r_str_trim_dup (input);
	int n = r_str_word_set0 (ptr);
	if (!n) {
		// get boundaries of current memory map, section or io map
		if (cfg_debug) {
			RDebugMap *map = r_debug_map_get (core->dbg, core->offset);
			if (map) {
				from = map->addr;
				to = map->addr_end;
			}
		} else {
			RList *list = r_core_get_boundaries_prot (core, R_PERM_X, NULL, "anal");
			RListIter *iter;
			RIOMap* map;
			if (!list) {
				return 0;
			}
			if (rad == 'j') {
				r_cons_printf ("{");
			}
			int nth = 0;
			r_list_foreach (list, iter, map) {
				from = map->itv.addr;
				to = r_itv_end (map->itv);
				if (r_cons_is_breaked ()) {
					break;
				}
				if (!from && !to) {
					eprintf ("Cannot determine xref search boundaries\n");
				} else if (to - from > UT32_MAX) {
					eprintf ("Skipping huge range\n");
				} else {
					if (rad == 'j') {
						r_cons_printf ("%s\"mapid\":\"%d\",\"refs\":{", nth? ",": "", map->id);
					}
					r_core_anal_search_xrefs (core, from, to, rad);
					if (rad == 'j') {
						r_cons_printf ("}");
					}
					nth++;
				}
			}
			if (rad == 'j') {
				r_cons_printf ("}\n");
			}
			free (ptr);
			r_list_free (list);
			return 1;
		}
	} else if (n == 1) {
		from = core->offset;
		to = core->offset + r_num_math (core->num, r_str_word_get0 (ptr, 0));
	} else {
		eprintf ("Invalid number of arguments\n");
	}
	free (ptr);

	if (from == UT64_MAX && to == UT64_MAX) {
		return false;
	}
	if (!from && !to) {
		return false;
	}
	if (to - from > r_io_size (core->io)) {
		return false;
	}
	if (rad == 'j') {
		r_cons_printf ("{");
	}
	bool res = r_core_anal_search_xrefs (core, from, to, rad);
	if (rad == 'j') {
		r_cons_printf ("}\n");
	}
	return res;
}