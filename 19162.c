R_API int r_core_anal_all(RCore *core) {
	RList *list;
	RListIter *iter;
	RFlagItem *item;
	RAnalFunction *fcni;
	RBinAddr *binmain;
	RBinAddr *entry;
	RBinSymbol *symbol;
	const bool anal_vars = r_config_get_i (core->config, "anal.vars");
	const bool anal_calls = r_config_get_i (core->config, "anal.calls");

	/* Analyze Functions */
	/* Entries */
	item = r_flag_get (core->flags, "entry0");
	if (item) {
		r_core_af (core, item->offset, "entry0", anal_calls);
	} else {
		r_core_af (core, core->offset, NULL, anal_calls);
	}

	r_core_task_yield (&core->tasks);

	r_cons_break_push (NULL, NULL);
	/* Symbols (Imports are already analyzed by rabin2 on init) */
	if ((list = r_bin_get_symbols (core->bin)) != NULL) {
		r_list_foreach (list, iter, symbol) {
			if (r_cons_is_breaked ()) {
				break;
			}
			// Stop analyzing PE imports further
			if (isSkippable (symbol)) {
				continue;
			}
			if (isValidSymbol (symbol)) {
				ut64 addr = r_bin_get_vaddr (core->bin, symbol->paddr, symbol->vaddr);
				// TODO: uncomment to: fcn.name = symbol.name, problematic for imports
				// r_core_af (core, addr, symbol->name, anal_calls);
				r_core_af (core, addr, NULL, anal_calls);
			}
		}
	}
	r_core_task_yield (&core->tasks);
	/* Main */
	if ((binmain = r_bin_get_sym (core->bin, R_BIN_SYM_MAIN))) {
		if (binmain->paddr != UT64_MAX) {
			ut64 addr = r_bin_get_vaddr (core->bin, binmain->paddr, binmain->vaddr);
			r_core_af (core, addr, "main", anal_calls);
		}
	}
	r_core_task_yield (&core->tasks);
	if ((list = r_bin_get_entries (core->bin))) {
		r_list_foreach (list, iter, entry) {
			if (r_cons_is_breaked ()) {
				break;
			}
			if (entry->paddr == UT64_MAX) {
				continue;
			}
			ut64 addr = r_bin_get_vaddr (core->bin, entry->paddr, entry->vaddr);
			r_core_af (core, addr, NULL, anal_calls);
		}
	}
	r_core_task_yield (&core->tasks);
	if (anal_vars) {
		/* Set fcn type to R_ANAL_FCN_TYPE_SYM for symbols */
		r_list_foreach_prev (core->anal->fcns, iter, fcni) {
			if (r_cons_is_breaked ()) {
				break;
			}
			r_core_recover_vars (core, fcni, true);
			if (!strncmp (fcni->name, "dbg.", 4) || !strncmp (fcni->name, "sym.", 4) || !strncmp (fcni->name, "main", 4)) {
				fcni->type = R_ANAL_FCN_TYPE_SYM;
			}
		}
	}
	r_cons_break_pop ();
	return true;
}