static void cmd_anal_calls(RCore *core, const char *input, bool printCommands, bool importsOnly) {
	RList *ranges = NULL;
	RIOMap *r;
	ut64 addr;
	ut64 len = r_num_math (core->num, input);
	if (len > 0xffffff) {
		eprintf ("Too big\n");
		return;
	}
	RBinFile *binfile = r_bin_cur (core->bin);
	addr = core->offset;
	if (binfile) {
		if (len) {
			RIOMap *m = R_NEW0 (RIOMap);
			m->itv.addr = addr;
			m->itv.size = len;
			ranges = r_list_newf ((RListFree)free);
			r_list_append (ranges, m);
		} else {
			ranges = r_core_get_boundaries_prot (core, R_PERM_X, NULL, "anal");
		}
	}
	r_cons_break_push (NULL, NULL);
	if (!binfile || (ranges && !r_list_length (ranges))) {
		RListIter *iter;
		RIOMap *map;
		r_list_free (ranges);
		ranges = r_core_get_boundaries_prot (core, 0, NULL, "anal");
		if (ranges) {
			r_list_foreach (ranges, iter, map) {
				ut64 addr = map->itv.addr;
				_anal_calls (core, addr, r_itv_end (map->itv), printCommands, importsOnly);
			}
		}
	} else {
		RListIter *iter;
		if (binfile) {
			r_list_foreach (ranges, iter, r) {
				addr = r->itv.addr;
				//this normally will happen on fuzzed binaries, dunno if with huge
				//binaries as well
				if (r_cons_is_breaked ()) {
					break;
				}
				_anal_calls (core, addr, r_itv_end (r->itv), printCommands, importsOnly);
			}
		}
	}
	r_cons_break_pop ();
	r_list_free (ranges);
}