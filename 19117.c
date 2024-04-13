R_API int r_core_anal_fcn_list(RCore *core, const char *input, const char *rad) {
	char temp[64];
	r_return_val_if_fail (core && core->anal, 0);
	if (r_list_empty (core->anal->fcns)) {
		if (*rad == 'j') {
			r_cons_println ("[]");
		}
		return 0;
	}
	if (*rad == '.') {
		RList *fcns = r_anal_get_functions_in (core->anal, core->offset);
		if (!fcns || r_list_empty (fcns)) {
			eprintf ("No functions at current address.\n");
			r_list_free (fcns);
			return -1;
		}
		fcn_list_default (core, fcns, false);
		r_list_free (fcns);
		return 0;
	}

	if (rad && (*rad == 'l' || *rad == 'j')) {
		fcnlist_gather_metadata (core->anal, core->anal->fcns);
	}

	const char *name = input;
	ut64 addr = core->offset;
	if (input && *input) {
		name = input + 1;
		addr = r_num_math (core->num, name);
	}

	RList *fcns = r_list_newf (NULL);
	if (!fcns) {
		return -1;
	}
	RListIter *iter;
	RAnalFunction *fcn;
	r_list_foreach (core->anal->fcns, iter, fcn) {
		if (!input || r_anal_function_contains (fcn, addr) || (!strcmp (name, fcn->name))) {
			r_list_append (fcns, fcn);
		}
	}

	// Use afls[asn] to sort by address, size or name, dont sort it here .. r_list_sort (fcns, &cmpfcn);
	if (!rad) {
		fcn_list_default (core, fcns, false);
		r_list_free (fcns);
		return 0;
	}
	switch (*rad) {
	case '+':
		r_core_anal_fcn_list_size (core);
		break;
	case '=': { // afl=
		r_list_sort (fcns, cmpaddr);
		RList *flist = r_list_newf ((RListFree) r_listinfo_free);
		if (!flist) {
			r_list_free (fcns);
			return -1;
		}
		ls_foreach (fcns, iter, fcn) {
			RInterval inter = {r_anal_function_min_addr (fcn), r_anal_function_linear_size (fcn) };
			RListInfo *info = r_listinfo_new (r_core_anal_fcn_name (core, fcn), inter, inter, -1, sdb_itoa (fcn->bits, temp, 10));
			if (!info) {
				break;
			}
			r_list_append (flist, info);
		}
		RTable *table = r_core_table (core, "functions");
		r_table_visual_list (table, flist, core->offset, core->blocksize,
			r_cons_get_size (NULL), r_config_get_i (core->config, "scr.color"));
		r_cons_printf ("\n%s\n", r_table_tostring (table));
		r_table_free (table);
		r_list_free (flist);
		break;
		}
	case ',': // "afl," "afl,j"
	case 't': // "aflt" "afltj"
		if (rad[1] == 'j') {
			fcn_list_table (core, r_str_trim_head_ro (rad + 2), 'j');
		} else {
			fcn_list_table (core, r_str_trim_head_ro (rad + 1), rad[1]);
		}
		break;
	case 'l': // "afll" "afllj"
		if (rad[1] == 'j') {
			fcn_list_verbose_json (core, fcns);
		} else {
			char *sp = strchr (rad, ' ');
			fcn_list_verbose (core, fcns, sp?sp+1: NULL);
		}
		break;
	case 'q':
		if (rad[1] == 'j') {
			fcn_list_json (core, fcns, true);
		} else {
			fcn_list_default (core, fcns, true);
		}
		break;
	case 'j':
		fcn_list_json (core, fcns, false);
		break;
	case '*':
		fcn_list_detail (core, fcns);
		break;
	case 'm': // "aflm"
		{
			char mode = 'm';
			if (rad[1] != 0) {
				if (rad[1] == 'j') { // "aflmj"
					mode = 'j';
				} else if (rad[1] == 'q') { // "aflmq"
					mode = 'q';
				}
			}
			fcn_print_makestyle (core, fcns, mode);
			break;
		}
	case 1:
		fcn_list_legacy (core, fcns);
		break;
	default:
		fcn_list_default (core, fcns, false);
		break;
	}
	r_list_free (fcns);
	return 0;
}