static int cmd_anal_all(RCore *core, const char *input) {
	switch (*input) {
	case '?':
		r_core_cmd_help (core, help_msg_aa);
		break;
	case 'b': // "aab"
		cmd_anal_blocks (core, input + 1);
		break;
	case 'f':
		if (input[1] == 'e') {  // "aafe"
			r_core_cmd0 (core, "aef@@f");
		} else if (input[1] == 'r') {
			ut64 cur = core->offset;
			bool hasnext = r_config_get_i (core->config, "anal.hasnext");
			RListIter *iter;
			RIOMap *map;
			RList *list = r_core_get_boundaries_prot (core, R_PERM_X, NULL, "anal");
			if (!list) {
				break;
			}
			r_list_foreach (list, iter, map) {
				r_core_seek (core, map->itv.addr, 1);
				r_config_set_i (core->config, "anal.hasnext", 1);
				r_core_cmd0 (core, "afr");
				r_config_set_i (core->config, "anal.hasnext", hasnext);
			}
			r_list_free (list);
			r_core_seek (core, cur, 1);
		} else if (input[1] == 't') { // "aaft"
			cmd_anal_aaft (core);
		} else if (input[1] == 0) { // "aaf"
			const bool analHasnext = r_config_get_i (core->config, "anal.hasnext");
			r_config_set_i (core->config, "anal.hasnext", true);
			r_core_cmd0 (core, "afr@@c:isq");
			r_config_set_i (core->config, "anal.hasnext", analHasnext);
		} else {
			r_cons_printf ("Usage: aaf[e|r|t] - analyze all functions again\n");
			r_cons_printf (" aafe = aef@@f\n");
			r_cons_printf ("aafr [len] = analyze all consecutive functions in section\n");
			r_cons_printf (" aaft = recursive type matching in all functions\n");
			r_cons_printf (" aaf  = afr@@c:isq\n");
		}
		break;
	case 'c': // "aac"
		switch (input[1]) {
		case '*': // "aac*"
			cmd_anal_calls (core, input + 1, true, false);
			break;
		case 'i': // "aaci"
			cmd_anal_calls (core, input + 1, input[2] == '*', true);
			break;
		case '?': // "aac?"
			eprintf ("Usage: aac, aac* or aaci (imports xrefs only)\n");
			break;
		default: // "aac"
			cmd_anal_calls (core, input + 1, false, false);
			break;
		}
	case 'j': // "aaj"
		cmd_anal_jumps (core, input + 1);
		break;
	case 'd': // "aad"
		cmd_anal_aad (core, input);
		break;
	case 'v': // "aav"
		cmd_anal_aav (core, input);
		break;
	case 'u': // "aau" - print areas not covered by functions
		r_core_anal_nofunclist (core, input + 1);
		break;
	case 'i': // "aai"
		r_core_anal_info (core, input + 1);
		break;
	case 's': // "aas"
		r_core_cmd0 (core, "af @@= `isq~[0]`");
		r_core_cmd0 (core, "af @@ entry*");
		break;
	case 'S': // "aaS"
		r_core_cmd0 (core, "af @@ sym.*");
		r_core_cmd0 (core, "af @@ entry*");
		break;
	case 'F': // "aaF" "aaFa"
		if (!input[1] || input[1] == ' ' || input[1] == 'a') {
			r_core_anal_inflags (core, input + 1);
		} else {
			eprintf ("Usage: aaF[a] - analyze functions in flag bounds (aaFa uses af/a2f instead of af+/afb+)\n");
		}
		break;
	case 'n': // "aan"
		switch (input[1]) {
		case 'r': // "aanr" // all noreturn propagation
			r_core_anal_propagate_noreturn (core);
			break;
		case 'g': // "aang"
			r_core_anal_autoname_all_golang_fcns (core);
			break;
		case '?':
			eprintf ("Usage: aan[rg]\n");
			eprintf ("aan  : autoname all functions\n");
			eprintf ("aang : autoname all golang functions\n");
			eprintf ("aanr : auto-noreturn propagation\n");
			break;
		default: // "aan"
			r_core_anal_autoname_all_fcns (core);
		}
		break;
	case 'p': // "aap"
		if (input[1] == '?') {
			// TODO: accept parameters for ranges
			eprintf ("Usage: /aap   ; find in memory for function preludes");
		} else {
			r_core_search_preludes (core, true);
		}
		break;
	case '\0': // "aa"
	case 'a':
		if (input[0] && (input[1] == '?' || (input[1] && input[2] == '?'))) {
			r_cons_println ("Usage: See aa? for more help");
		} else {
			bool didAap = false;
			char *dh_orig = NULL;
			if (!strncmp (input, "aaaaa", 5)) {
				eprintf ("An r2 developer is coming to your place to manually analyze this program. Please wait for it\n");
				if (r_cons_is_interactive ()) {
					r_cons_any_key (NULL);
				}
				goto jacuzzi;
			}
			ut64 curseek = core->offset;
			oldstr = r_print_rowlog (core->print, "Analyze all flags starting with sym. and entry0 (aa)");
			r_cons_break_push (NULL, NULL);
			r_cons_break_timeout (r_config_get_i (core->config, "anal.timeout"));
			r_core_anal_all (core);
			r_print_rowlog_done (core->print, oldstr);
			r_core_task_yield (&core->tasks);
			// Run pending analysis immediately after analysis
			// Usefull when running commands with ";" or via r2 -c,-i
			dh_orig = core->dbg->h
				? strdup (core->dbg->h->name)
				: strdup ("esil");
			if (core->io && core->io->desc && core->io->desc->plugin && !core->io->desc->plugin->isdbg) {
				//use dh_origin if we are debugging
				R_FREE (dh_orig);
			}
			if (r_cons_is_breaked ()) {
				goto jacuzzi;
			}
			r_cons_clear_line (1);
			bool cfg_debug = r_config_get_i (core->config, "cfg.debug");
			if (*input == 'a') { // "aaa"
				if (r_str_startswith (r_config_get (core->config, "bin.lang"), "go")) {
					oldstr = r_print_rowlog (core->print, "Find function and symbol names from golang binaries (aang)");
					r_print_rowlog_done (core->print, oldstr);
					r_core_anal_autoname_all_golang_fcns (core);
					oldstr = r_print_rowlog (core->print, "Analyze all flags starting with sym.go. (aF @@ sym.go.*)");
					r_core_cmd0 (core, "aF @@ sym.go.*");
					r_print_rowlog_done (core->print, oldstr);
				}
				r_core_task_yield (&core->tasks);
				if (!cfg_debug) {
					if (dh_orig && strcmp (dh_orig, "esil")) {
						r_core_cmd0 (core, "dL esil");
						r_core_task_yield (&core->tasks);
					}
				}
				int c = r_config_get_i (core->config, "anal.calls");
				r_config_set_i (core->config, "anal.calls", 1);
				r_core_cmd0 (core, "s $S");
				if (r_cons_is_breaked ()) {
					goto jacuzzi;
				}

				oldstr = r_print_rowlog (core->print, "Analyze function calls (aac)");
				(void)cmd_anal_calls (core, "", false, false); // "aac"
				r_core_seek (core, curseek, 1);
				// oldstr = r_print_rowlog (core->print, "Analyze data refs as code (LEA)");
				// (void) cmd_anal_aad (core, NULL); // "aad"
				r_print_rowlog_done (core->print, oldstr);
				r_core_task_yield (&core->tasks);
				if (r_cons_is_breaked ()) {
					goto jacuzzi;
				}

				if (is_unknown_file (core)) {
					oldstr = r_print_rowlog (core->print, "find and analyze function preludes (aap)");
					(void)r_core_search_preludes (core, false); // "aap"
					didAap = true;
					r_print_rowlog_done (core->print, oldstr);
					r_core_task_yield (&core->tasks);
					if (r_cons_is_breaked ()) {
						goto jacuzzi;
					}
				}

				oldstr = r_print_rowlog (core->print, "Analyze len bytes of instructions for references (aar)");
				(void)r_core_anal_refs (core, ""); // "aar"
				r_print_rowlog_done (core->print, oldstr);
				r_core_task_yield (&core->tasks);
				if (r_cons_is_breaked ()) {
					goto jacuzzi;
				}
				oldstr = r_print_rowlog (core->print, "Check for objc references");
				r_print_rowlog_done (core->print, oldstr);
				cmd_anal_objc (core, input + 1, true);
				r_core_task_yield (&core->tasks);
				oldstr = r_print_rowlog (core->print, "Check for vtables");
				r_core_cmd0 (core, "avrr");
				r_print_rowlog_done (core->print, oldstr);
				r_core_task_yield (&core->tasks);
				r_config_set_i (core->config, "anal.calls", c);
				r_core_task_yield (&core->tasks);
				if (r_cons_is_breaked ()) {
					goto jacuzzi;
				}
				if (!r_str_startswith (r_config_get (core->config, "asm.arch"), "x86")) {
					r_core_cmd0 (core, "aav");
					r_core_task_yield (&core->tasks);
					bool ioCache = r_config_get_i (core->config, "io.pcache");
					r_config_set_i (core->config, "io.pcache", 1);
					oldstr = r_print_rowlog (core->print, "Emulate code to find computed references (aae)");
					r_core_cmd0 (core, "aae");
					r_print_rowlog_done (core->print, oldstr);
					r_core_task_yield (&core->tasks);
					if (!ioCache) {
						r_core_cmd0 (core, "wc-*");
						r_core_task_yield (&core->tasks);
					}
					r_config_set_i (core->config, "io.pcache", ioCache);
					if (r_cons_is_breaked ()) {
						goto jacuzzi;
					}
				}
				if (r_config_get_i (core->config, "anal.autoname")) {
					oldstr = r_print_rowlog (core->print, "Speculatively constructing a function name "
					                         "for fcn.* and sym.func.* functions (aan)");
					r_core_anal_autoname_all_fcns (core);
					r_print_rowlog_done (core->print, oldstr);
					r_core_task_yield (&core->tasks);
				}
				if (core->anal->opt.vars) {
					RAnalFunction *fcni;
					RListIter *iter;
					r_list_foreach (core->anal->fcns, iter, fcni) {
						if (r_cons_is_breaked ()) {
							break;
						}
						RList *list = r_anal_var_list (core->anal, fcni, 'r');
						if (!r_list_empty (list)) {
							r_list_free (list);
							continue;
						}
						//extract only reg based var here
						r_core_recover_vars (core, fcni, true);
						r_list_free (list);
					}
					r_core_task_yield (&core->tasks);
				}
				if (!sdb_isempty (core->anal->sdb_zigns)) {
					oldstr = r_print_rowlog (core->print, "Check for zignature from zigns folder (z/)");
					r_core_cmd0 (core, "z/");
					r_print_rowlog_done (core->print, oldstr);
					r_core_task_yield (&core->tasks);
				}

				oldstr = r_print_rowlog (core->print, "Type matching analysis for all functions (aaft)");
				r_core_cmd0 (core, "aaft");
				r_print_rowlog_done (core->print, oldstr);
				r_core_task_yield (&core->tasks);

				oldstr = r_print_rowlog (core->print, "Propagate noreturn information");
				r_core_anal_propagate_noreturn (core);
				r_print_rowlog_done (core->print, oldstr);
				r_core_task_yield (&core->tasks);

				oldstr = r_print_rowlog (core->print, "Use -AA or aaaa to perform additional experimental analysis.");
				r_print_rowlog_done (core->print, oldstr);

				if (input[1] == 'a') { // "aaaa"
					if (!didAap) {
						oldstr = r_print_rowlog (core->print, "Finding function preludes");
						(void)r_core_search_preludes (core, false); // "aap"
						r_print_rowlog_done (core->print, oldstr);
						r_core_task_yield (&core->tasks);
					}

					oldstr = r_print_rowlog (core->print, "Enable constraint types analysis for variables");
					r_config_set (core->config, "anal.types.constraint", "true");
					r_print_rowlog_done (core->print, oldstr);
				}
				r_core_cmd0 (core, "s-");
				if (dh_orig) {
					r_core_cmdf (core, "dL %s", dh_orig);
					r_core_task_yield (&core->tasks);
				}
			}
			r_core_seek (core, curseek, 1);
		jacuzzi:
			// XXX this shouldnt be called. flags muts be created wheen the function is registered
			flag_every_function (core);
			r_cons_break_pop ();
			R_FREE (dh_orig);
		}
		break;
	case 't': { // "aat"
		char *off = input[1]? r_str_trim_dup (input + 2): NULL;
		RAnalFunction *fcn;
		RListIter *it;
		if (off && *off) {
			ut64 addr = r_num_math (NULL, off);
			fcn = r_anal_get_function_at (core->anal, core->offset);
			if (fcn) {
				r_core_link_stroff (core, fcn);
			} else {
				eprintf ("Cannot find function at %08" PFMT64x "\n", addr);
			}
		} else {
			if (r_list_empty (core->anal->fcns)) {
				eprintf ("Couldn't find any functions\n");
				break;
			}
			r_list_foreach (core->anal->fcns, it, fcn) {
				if (r_cons_is_breaked ()) {
					break;
				}
				r_core_link_stroff (core, fcn);
			}
		}
		free (off);
		break;
	}
	case 'T': // "aaT"
		cmd_anal_aftertraps (core, input + 1);
		break;
	case 'o': // "aao"
		cmd_anal_objc (core, input + 1, false);
		break;
	case 'e': // "aae"
		if (input[1]) {
			const char *len = (char *)input + 1;
			char *addr = strchr (input + 2, ' ');
			if (addr) {
				*addr++ = 0;
			}
			r_core_anal_esil (core, len, addr);
		} else {
			ut64 at = core->offset;
			RIOMap *map;
			RListIter *iter;
			RList *list = r_core_get_boundaries_prot (core, -1, NULL, "anal");
			if (!list) {
				break;
			}
			r_list_foreach (list, iter, map) {
				r_core_seek (core, map->itv.addr, 1);
				r_core_anal_esil (core, "$SS", NULL);
			}
			r_list_free (list);
			r_core_seek (core, at, 1);
		}
		break;
	case 'r':
		(void)r_core_anal_refs (core, input + 1);
		break;
	default:
		r_core_cmd_help (core, help_msg_aa);
		break;
	}

	return true;
}