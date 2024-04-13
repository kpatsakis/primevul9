static void cmd_anal_aav(RCore *core, const char *input) {
#define seti(x,y) r_config_set_i(core->config, x, y);
#define geti(x) r_config_get_i(core->config, x);
	r_return_if_fail (*input == 'v');
	ut64 o_align = geti ("search.align");
	const char *analin =  r_config_get (core->config, "anal.in");
	char *tmp = strdup (analin);
	bool asterisk = strchr (input, '*');
	bool is_debug = r_config_get_i (core->config, "cfg.debug");
	int archAlign = r_anal_archinfo (core->anal, R_ANAL_ARCHINFO_ALIGN);
	seti ("search.align", archAlign);
	r_config_set (core->config, "anal.in", "io.maps.x");
	oldstr = r_print_rowlog (core->print, "Finding xrefs in noncode section with anal.in=io.maps");
	r_print_rowlog_done (core->print, oldstr);

	int vsize = 4; // 32bit dword
	if (core->assembler->bits == 64) {
		vsize = 8;
	}

	// body
	oldstr = r_print_rowlog (core->print, "Analyze value pointers (aav)");
	r_print_rowlog_done (core->print, oldstr);
	r_cons_break_push (NULL, NULL);
	if (is_debug) {
		RList *list = r_core_get_boundaries_prot (core, 0, "dbg.map", "anal");
		RListIter *iter;
		RIOMap *map;
		if (!list) {
			goto beach;
		}
		r_list_foreach (list, iter, map) {
			if (r_cons_is_breaked ()) {
				break;
			}
			oldstr = r_print_rowlog (core->print, sdb_fmt ("from 0x%"PFMT64x" to 0x%"PFMT64x" (aav)", map->itv.addr, r_itv_end (map->itv)));
			r_print_rowlog_done (core->print, oldstr);
			(void)r_core_search_value_in_range (core, map->itv,
				map->itv.addr, r_itv_end (map->itv), vsize, _CbInRangeAav, (void *)asterisk);
		}
		r_list_free (list);
	} else {
		RList *list = r_core_get_boundaries_prot (core, 0, NULL, "anal");
		if (!list) {
			goto beach;
		}
		RListIter *iter, *iter2;
		RIOMap *map, *map2;
		ut64 from = UT64_MAX;
		ut64 to = UT64_MAX;
		// find values pointing to non-executable regions
		r_list_foreach (list, iter2, map2) {
			if (r_cons_is_breaked ()) {
				break;
			}
			//TODO: Reduce multiple hits for same addr
			from = r_itv_begin (map2->itv);
			to = r_itv_end (map2->itv);
			oldstr = r_print_rowlog (core->print, sdb_fmt ("Value from 0x%08"PFMT64x " to 0x%08" PFMT64x " (aav)", from, to));
			r_print_rowlog_done (core->print, oldstr);
			r_list_foreach (list, iter, map) {
				ut64 begin = map->itv.addr;
				ut64 end = r_itv_end (map->itv);
				if (r_cons_is_breaked ()) {
					break;
				}
				if (end - begin > UT32_MAX) {
					oldstr = r_print_rowlog (core->print, "Skipping huge range");
					r_print_rowlog_done (core->print, oldstr);
					continue;
				}
				oldstr = r_print_rowlog (core->print, sdb_fmt ("0x%08"PFMT64x"-0x%08"PFMT64x" in 0x%"PFMT64x"-0x%"PFMT64x" (aav)", from, to, begin, end));
				r_print_rowlog_done (core->print, oldstr);
				(void)r_core_search_value_in_range (core, map->itv, from, to, vsize, _CbInRangeAav, (void *)asterisk);
			}
		}
		r_list_free (list);
	}
beach:
	r_cons_break_pop ();
	// end
	r_config_set (core->config, "anal.in", tmp);
	free (tmp);
	seti ("search.align", o_align);
}