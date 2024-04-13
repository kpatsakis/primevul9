static int cmd_anal_fcn(RCore *core, const char *input) {
	char i;

	r_cons_break_timeout (r_config_get_i (core->config, "anal.timeout"));
	switch (input[1]) {
	case '-': // "af-"
		if (!input[2]) {
			cmd_anal_fcn (core, "f-$$");
			r_core_anal_undefine (core, core->offset);
		} else if (!strcmp (input + 2, "*")) {
			RAnalFunction *f;
			RListIter *iter;
			r_list_foreach (core->anal->fcns, iter, f) {
				r_anal_del_jmprefs (core->anal, f);
			}
			r_list_purge (core->anal->fcns);
		} else {
			ut64 addr = input[2]
				? r_num_math (core->num, input + 2)
				: core->offset;
			r_core_anal_undefine (core, addr);
			r_anal_fcn_del_locs (core->anal, addr);
			r_anal_fcn_del (core->anal, addr);
		}
		break;
	case 'j': // "afj"
		{
			RList *blocks = r_anal_get_blocks_in (core->anal, core->offset);
			RAnalBlock *block = r_list_first (blocks);
			if (block && !r_list_empty (block->fcns)) {
				char *args = strdup (input + 2);
				RList *argv = r_str_split_list (args, " ", 0);
				ut64 table = r_num_math (core->num, r_list_get_n (argv, 0));
				ut64 elements = r_num_math (core->num, r_list_get_n (argv, 1));
				r_anal_jmptbl (core->anal, r_list_first (block->fcns), block, core->offset, table, elements, UT64_MAX);
			} else {
				eprintf ("No function defined here\n");
			}
			r_list_free (blocks);
		}
		break;
	case 'a': // "afa"
		if (input[2] == 'l') { // "afal" : list function call arguments
			int show_args = r_config_get_i (core->config, "dbg.funcarg");
			if (show_args) {
				r_core_print_func_args (core);
			}
		} else {
			r_core_print_func_args (core);
		}
		break;
	case 'd': // "afd"
		{
		ut64 addr = 0;
		if (input[2] == '?') {
			eprintf ("afd [offset]\n");
		} else if (input[2] == ' ') {
			addr = r_num_math (core->num, input + 2);
		} else {
			addr = core->offset;
		}
		RAnalFunction *fcn = r_anal_get_fcn_in (core->anal, addr, 0);
		if (input[2] == 'j') { // afdj
			PJ *pj = pj_new ();
			if (!pj) {
				return false;
			}
			pj_o (pj);
			if (fcn) {
				pj_ks (pj, "name", fcn->name);
				pj_ki (pj, "offset", (int)(addr - fcn->addr));
			}
			pj_end (pj);
			r_cons_println (pj_string (pj));
			pj_free (pj);
		} else {
			if (fcn) {
				if (fcn->addr != addr) {
					r_cons_printf ("%s + %d\n", fcn->name,
							(int)(addr - fcn->addr));
				} else {
					r_cons_println (fcn->name);
				}
			} else {
				eprintf ("afd: Cannot find function\n");
			}
		}
		}
		break;
	case 'u': // "afu"
		{
		if (input[2] != ' ') {
			eprintf ("Missing argument\n");
			return false;
		}

		ut64 addr = core->offset;
		ut64 addr_end = r_num_math (core->num, input + 2);
		if (addr_end < addr) {
			eprintf ("Invalid address ranges\n");
		} else {
			int depth = 1;
			ut64 a, b;
			const char *c;
			a = r_config_get_i (core->config, "anal.from");
			b = r_config_get_i (core->config, "anal.to");
			c = r_config_get (core->config, "anal.limits");
			r_config_set_i (core->config, "anal.from", addr);
			r_config_set_i (core->config, "anal.to", addr_end);
			r_config_set (core->config, "anal.limits", "true");

			RAnalFunction *fcn = r_anal_get_fcn_in (core->anal, addr, 0);
			if (fcn) {
				r_anal_fcn_resize (core->anal, fcn, addr_end - addr);
			}
			r_core_anal_fcn (core, addr, UT64_MAX,
					R_ANAL_REF_TYPE_NULL, depth);
			fcn = r_anal_get_fcn_in (core->anal, addr, 0);
			if (fcn) {
				r_anal_fcn_resize (core->anal, fcn, addr_end - addr);
			}
			r_config_set_i (core->config, "anal.from", a);
			r_config_set_i (core->config, "anal.to", b);
			r_config_set (core->config, "anal.limits", c? c: "");
		}
		}
		break;
	case '+': { // "af+"
		if (input[2] != ' ') {
			eprintf ("Missing arguments\n");
			return false;
		}
		char *ptr = strdup (input + 3);
		const char *ptr2;
		int n = r_str_word_set0 (ptr);
		const char *name = NULL;
		ut64 addr = UT64_MAX;
		RAnalDiff *diff = NULL;
		int type = R_ANAL_FCN_TYPE_FCN;
		if (n > 1) {
			switch (n) {
			case 4:
				ptr2 = r_str_word_get0 (ptr, 3);
				if (!(diff = r_anal_diff_new ())) {
					eprintf ("error: Cannot init RAnalDiff\n");
					free (ptr);
					return false;
				}
				if (ptr2[0] == 'm') {
					diff->type = R_ANAL_DIFF_TYPE_MATCH;
				} else if (ptr2[0] == 'u') {
					diff->type = R_ANAL_DIFF_TYPE_UNMATCH;
				}
			case 3:
				ptr2 = r_str_word_get0 (ptr, 2);
				if (strchr (ptr2, 'l')) {
					type = R_ANAL_FCN_TYPE_LOC;
				} else if (strchr (ptr2, 'i')) {
					type = R_ANAL_FCN_TYPE_IMP;
				} else if (strchr (ptr2, 's')) {
					type = R_ANAL_FCN_TYPE_SYM;
				} else {
					type = R_ANAL_FCN_TYPE_FCN;
				}
			case 2:
				name = r_str_word_get0 (ptr, 1);
			case 1:
				addr = r_num_math (core->num, r_str_word_get0 (ptr, 0));
			}
			RAnalFunction *fcn = r_anal_create_function (core->anal, name, addr, type, diff);
			if (!fcn) {
				eprintf ("Cannot add function (duplicated)\n");
			}
		}
		r_anal_diff_free (diff);
		free (ptr);
		}
		break;
	case 'o': // "afo"
		switch (input[2]) {
		case '?':
			eprintf ("Usage: afo[?sj] ([name|offset])\n");
			break;
		case 'j':
			{
				RAnalFunction *fcn = r_anal_get_fcn_in (core->anal, core->offset, R_ANAL_FCN_TYPE_NULL);
				PJ *pj = pj_new ();
				if (!pj) {
					return false;
				}
				pj_o (pj);
				if (fcn) {
					pj_ki (pj, "address", fcn->addr);
				}
				pj_end (pj);
				r_cons_println (pj_string (pj));
				pj_free (pj);
			}
			break;
		case '\0':
			{
				RAnalFunction *fcn = r_anal_get_fcn_in (core->anal, core->offset, R_ANAL_FCN_TYPE_NULL);
				if (fcn) {
					r_cons_printf ("0x%08" PFMT64x "\n", fcn->addr);
				}
			}
			break;
		case 's': // "afos"
			{
				ut64 addr = core->offset;
				RListIter *iter;
				RList *list = r_anal_get_functions_in (core->anal, addr);
				RAnalFunction *fcn;
				r_list_foreach (list, iter, fcn) {
					r_cons_printf ("= 0x%08" PFMT64x "\n", fcn->addr);
				}
				r_list_free (list);
			}
			break;
		case ' ':
			{
				RAnalFunction *fcn;
				ut64 addr = r_num_math (core->num, input + 3);
				if (addr == 0LL) {
					fcn = r_anal_fcn_find_name (core->anal, input + 3);
				} else {
					fcn = r_anal_get_fcn_in (core->anal, addr, R_ANAL_FCN_TYPE_NULL);
				}
				if (fcn) {
					r_cons_printf ("0x%08" PFMT64x "\n", fcn->addr);
				}
			}
			break;
		}
		break;
	case 'i': // "afi"
		switch (input[2]) {
		case '?':
			r_core_cmd_help (core, help_msg_afi);
			break;
		case '.': // "afi."
			{
				ut64 addr = core->offset;
				if (input[3] == ' ') {
					addr = r_num_math (core->num, input + 3);
				}
				RAnalFunction *fcn = r_anal_get_fcn_in (core->anal, addr, R_ANAL_FCN_TYPE_NULL);
				if (fcn) {
					r_cons_printf ("%s\n", fcn->name);
				}
			}
			break;
		case 'l': // "afil"
			if (input[3] == '?') {
				// TODO #7967 help refactor
				help_msg_afll[1] = "afil";
				r_core_cmd_help (core, help_msg_afll);
				break;
			}
			/* fallthrough */
		case 'i': // "afii"
			if (input[3] == '-') {
				RAnalFunction *fcn = r_anal_get_fcn_in (core->anal, core->offset, R_ANAL_FCN_TYPE_NULL);
				if (fcn) {
					r_list_free (fcn->imports);
					fcn->imports = NULL;
				}
			} else if (input[3] == ' ') {
				RAnalFunction *fcn = r_anal_get_fcn_in (core->anal, core->offset, R_ANAL_FCN_TYPE_NULL);
				if (fcn) {
					if (!fcn->imports) {
						fcn->imports = r_list_newf ((RListFree)free);
					}
					r_list_append (fcn->imports, r_str_trim_dup (input + 4));
				} else {
					eprintf ("No function found\n");
				}
			} else {
				RAnalFunction *fcn = r_anal_get_fcn_in (core->anal, core->offset, R_ANAL_FCN_TYPE_NULL);
				if (fcn && fcn->imports) {
					char *imp;
					RListIter *iter;
					r_list_foreach (fcn->imports, iter, imp) {
						r_cons_printf ("%s\n", imp);
					}
				}
			}
			break;
		case 's': // "afis"
			if (input[3] == 'a') { // "afisa"
				__core_cmd_anal_fcn_allstats (core, input + 4);
			} else {
				sdb_free (__core_cmd_anal_fcn_stats (core, input + 3));
			}
			break;
		case 'j': // "afij"
		case '*': // "afi*"
			r_core_anal_fcn_list (core, input + 3, input + 2);
			break;
		case 'p': // "afip"
			{
				RAnalFunction *fcn = r_anal_get_fcn_in (core->anal, core->offset, R_ANAL_FCN_TYPE_NULL);
				if (fcn) {
					r_cons_printf ("is-pure: %s\n", r_anal_fcn_get_purity (core->anal, fcn) ? "true" : "false");
				}
			}
			break;
		default:
			i = 1;
			r_core_anal_fcn_list (core, input + 2, &i);
			break;
		}
		break;
	case 'l': // "afl"
		switch (input[2]) {
		case '?':
			r_core_cmd_help (core, help_msg_afl);
			break;
		case 's': // "afls"
			switch (input[3]) {
			case '?':
				r_core_cmd_help (core, help_msg_afls);
				break;
			case 'a': // "aflsa"
				core->anal->fcns->sorted = false;
				r_list_sort (core->anal->fcns, cmpaddr);
				break;
			case 'b': // "aflsb"
				core->anal->fcns->sorted = false;
				r_list_sort (core->anal->fcns, cmpbbs);
				break;
			case 's': // "aflss"
				core->anal->fcns->sorted = false;
				r_list_sort (core->anal->fcns, cmpsize);
				break;
			case 'n': // "aflsn"
				core->anal->fcns->sorted = false;
				r_list_sort (core->anal->fcns, cmpname);
				break;
			default:
				core->anal->fcns->sorted = false;
				r_list_sort (core->anal->fcns, cmpaddr);
				break;
			}
			break;
		case 'l': // "afll"
			if (input[3] == '?') {
				// TODO #7967 help refactor
				help_msg_afll[1] = "afll";
				r_core_cmd_help (core, help_msg_afll);
				break;
			}
			/* fallthrough */
		case 't': // "aflt"
		case 'j': // "aflj"
		case 'q': // "aflq"
		case 'm': // "aflm"
		case '+': // "afl+"
		case '=': // "afl="
		case '*': // "afl*"
		case '.': // "afl*"
			r_core_anal_fcn_list (core, NULL, input + 2);
			break;
		case 'c': // "aflc"
			r_cons_printf ("%d\n", r_list_length (core->anal->fcns));
			break;
		default: // "afl "
			r_core_anal_fcn_list (core, NULL, "o");
			break;
		}
		break;
	case 's': // "afs"
		switch (input[2]) {
		case '!': { // "afs!"
			char *sig = r_core_cmd_str (core, "afs");
			char *data = r_core_editor (core, NULL, sig);
			if (sig && data) {
				r_core_cmdf (core, "\"afs %s\"", data);
			}
			free (sig);
			free (data);
			break;
		}
		case 'r': { // "afsr"
			RAnalFunction *fcn = r_anal_get_fcn_in (core->anal, core->offset, -1);
			if (fcn) {
				char *type = r_str_newf ("type.%s", input + 4);
				if (sdb_exists (core->anal->sdb_types, type)) {
					char *query = r_str_newf ("anal/types/func.%s.ret=%s", fcn->name, input + 4);
					sdb_querys (core->sdb, NULL, 0, query);
					free (query);
				}
				free (type);
			} else {
				eprintf ("There's no function defined in here.\n");
			}
			break;
		}
		case '*': // "afs*"
			eprintf ("TODO\n");
			break;
		case 'j': // "afsj"
			cmd_afsj (core, input + 2);
			break;
		case 0:
		case ' ': { // "afs"
			ut64 addr = core->offset;
			RAnalFunction *f;
			const char *arg = r_str_trim_head_ro (input + 2);
			if ((f = r_anal_get_fcn_in (core->anal, addr, R_ANAL_FCN_TYPE_NULL))) {
				if (arg && *arg) {
					// parse function signature here
					char *fcnstr = r_str_newf ("%s;", arg), *fcnstr_copy = strdup (fcnstr);
					char *fcnname_aux = strtok (fcnstr_copy, "(");
					r_str_trim_tail (fcnname_aux);
					char *fcnname = NULL;
					const char *ls = r_str_lchr (fcnname_aux, ' ');
					fcnname = strdup (ls? ls: fcnname_aux);
					if (fcnname) {
						// TODO: move this into r_anal_str_to_fcn()
						if (strcmp (f->name, fcnname)) {
							(void)__setFunctionName (core, addr, fcnname, false);
							f = r_anal_get_fcn_in (core->anal, addr, -1);
						}
						r_anal_str_to_fcn (core->anal, f, fcnstr);
					}
					free (fcnname);
					free (fcnstr_copy);
					free (fcnstr);
				} else {
					char *str = r_anal_function_get_signature (f);
					if (str) {
						r_cons_println (str);
						free (str);
					}
				}
			} else {
				eprintf ("No function defined at 0x%08" PFMT64x "\n", addr);
			}
			break;
		}
		default:
		// case '?': // "afs?"
			r_core_cmd_help (core, help_msg_afs);
			break;
		}
		break;
	case 'm': // "afm" - merge two functions
		r_core_anal_fcn_merge (core, core->offset, r_num_math (core->num, input + 2));
		break;
	case 'M': // "afM" - print functions map
		r_core_anal_fmap (core, input + 1);
		break;
	case 'v': // "afv"
		var_cmd (core, input + 2);
		break;
	case 't': // "aft"
		type_cmd (core, input + 2);
		break;
	case 'C': // "afC"
		if (input[2] == 'c') {
			RAnalFunction *fcn;
			if ((fcn = r_anal_get_fcn_in (core->anal, core->offset, 0)) != NULL) {
				r_cons_printf ("%i\n", r_anal_fcn_cc (core->anal, fcn));
			} else {
				eprintf ("Error: Cannot find function at 0x08%" PFMT64x "\n", core->offset);
			}
		} else if (input[2] == 'l') {
			RAnalFunction *fcn;
			if ((fcn = r_anal_get_fcn_in (core->anal, core->offset, 0)) != NULL) {
				r_cons_printf ("%d\n", r_anal_fcn_loops (fcn));
			} else {
				eprintf ("Error: Cannot find function at 0x08%" PFMT64x "\n", core->offset);
			}
		} else if (input[2] == '?') {
			r_core_cmd_help (core, help_msg_afC);
		} else {
			afCc (core, r_str_trim_head_ro (input + 2));
		}
		break;
	case 'c':{ // "afc"
		RAnalFunction *fcn = NULL;
		if (!input[2] || input[2] == ' ' || input[2] == 'r' || input[2] == 'a') {
			fcn = r_anal_get_fcn_in (core->anal, core->offset, 0);
			if (!fcn) {
				eprintf ("afc: Cannot find function here\n");
				break;
			}
		}
		switch (input[2]) {
		case '\0': // "afc"
			r_cons_println (fcn->cc);
			break;
		case ' ': { // "afc "
			char *argument = strdup (input + 3);
			char *cc = argument;
			r_str_trim (cc);
			if (!r_anal_cc_exist (core->anal, cc)) {
				const char *asmOs = r_config_get (core->config, "asm.os");
				eprintf ("afc: Unknown calling convention '%s' for '%s'\n"
						"See afcl for available types\n", cc, asmOs);
			} else {
				fcn->cc = r_str_constpool_get (&core->anal->constpool, cc);
			}
			free (argument);
			break;
		}
		case '=': // "afc="
			if (input[3]) {
				char *argument = strdup (input + 3);
				char *cc = argument;
				r_str_trim (cc);
				r_core_cmdf (core, "k anal/cc/default.cc=%s", cc);
				r_anal_set_reg_profile (core->anal);
				free (argument);
			} else {
				r_core_cmd0 (core, "k anal/cc/default.cc");
			}
			break;
		case 'a': // "afca"
			eprintf ("Todo\n");
			break;
		case 'f': // "afcf" "afcfj"
			cmd_anal_fcn_sig (core, input + 3);
			break;
		case 'k': // "afck"
			r_core_cmd0 (core, "k anal/cc/*");
			break;
		case 'l': // "afcl" list all function Calling conventions.
			sdb_foreach (core->anal->sdb_cc, cc_print, NULL);
			break;
		case 'o': { // "afco"
			char *dbpath = r_str_trim_dup (input + 3);
			if (r_file_exists (dbpath)) {
				Sdb *db = sdb_new (0, dbpath, 0);
				sdb_merge (core->anal->sdb_cc, db);
				sdb_close (db);
				sdb_free (db);
			}
			free (dbpath);
			break;
		}
		case 'r': {	// "afcr"
			int i;
			RStrBuf *json_buf = r_strbuf_new ("{");
			bool json = input[3] == 'j';

			char *cmd = r_str_newf ("cc.%s.ret", fcn->cc);
			const char *regname = sdb_const_get (core->anal->sdb_cc, cmd, 0);
			if (regname) {
				if (json) {
					r_strbuf_appendf (json_buf, "\"ret\":\"%s\"", regname);
				} else {
					r_cons_printf ("%s: %s\n", cmd, regname);
				}
			}
			free (cmd);

			bool isFirst = true;
			for (i = 0; i < R_ANAL_CC_MAXARG; i++) {
				cmd = r_str_newf ("cc.%s.arg%d", fcn->cc, i);
				regname = sdb_const_get (core->anal->sdb_cc, cmd, 0);
				if (regname) {
					if (json) {
						if (isFirst) {
							r_strbuf_appendf (json_buf, ",\"args\":[\"%s\"", regname);
							isFirst = false;
						} else {
							r_strbuf_appendf (json_buf, ",\"%s\"", regname);
						}
					} else {
						r_cons_printf ("%s: %s\n", cmd, regname);
					}
				}
				free (cmd);
			}
			if (!isFirst) {
				r_strbuf_append (json_buf, "]");
			}

			cmd = r_str_newf ("cc.%s.self", fcn->cc);
			regname = sdb_const_get (core->anal->sdb_cc, cmd, 0);
			if (regname) {
				if (json) {
					r_strbuf_appendf (json_buf, ",\"self\":\"%s\"", regname);
				} else {
					r_cons_printf ("%s: %s\n", cmd, regname);
				}
			}
			free (cmd);
			cmd = r_str_newf ("cc.%s.error", fcn->cc);
			regname = sdb_const_get (core->anal->sdb_cc, cmd, 0);
			if (regname) {
				if (json) {
					r_strbuf_appendf (json_buf, ",\"error\":\"%s\"", regname);
				} else {
					r_cons_printf ("%s: %s\n", cmd, regname);
				}
			}
			free (cmd);

			r_strbuf_append (json_buf, "}");
			if (json) {
				r_cons_printf ("%s\n", r_strbuf_drain (json_buf));
			}
		} break;
		case 'R': { // "afcR"
			/* very slow, but im tired of waiting for having this, so this is the quickest implementation */
			int i;
			char *cc = r_core_cmd_str (core, "k anal/cc/default.cc");
			r_str_trim (cc);
			for (i = 0; i < 6; i++) {
				char *res = r_core_cmd_strf (core, "k anal/cc/cc.%s.arg%d", cc, i);
				r_str_trim_nc (res);
				if (*res) {
					char *row = r_core_cmd_strf (core, "drr~%s 0x", res);
					r_str_trim (row);
					r_cons_printf ("arg[%d] %s\n", i, row);
					free (row);
				}
				free (res);
			}
			free (cc);
			}
			break;
		case '?': // "afc?"
		default:
			r_core_cmd_help (core, help_msg_afc);
		}
		}
		break;
	case 'B': // "afB" // set function bits
		if (input[2] == ' ') {
			RAnalFunction *fcn = r_anal_get_fcn_in (core->anal, core->offset, 0);
			if (fcn) {
				int bits = atoi (input + 3);
				r_anal_hint_set_bits (core->anal, r_anal_function_min_addr (fcn), bits);
				r_anal_hint_set_bits (core->anal, r_anal_function_max_addr (fcn), core->anal->bits);
				fcn->bits = bits;
			} else {
				eprintf ("afB: Cannot find function to set bits at 0x%08"PFMT64x"\n", core->offset);
			}
		} else {
			eprintf ("Usage: afB [bits]\n");
		}
		break;
	case 'b': // "afb"
		switch (input[2]) {
		case '-': // "afb-"
			anal_fcn_del_bb (core, input + 3);
			break;
		case 'e': // "afbe"
			anal_bb_edge (core, input + 3);
			break;
		case 'F': // "afbF"
			{
			RAnalFunction *fcn = r_anal_get_fcn_in (core->anal, core->offset, R_ANAL_FCN_TYPE_NULL);
			if (fcn) {
				RAnalBlock *bb = r_anal_fcn_bbget_in (core->anal, fcn, core->offset);
				if (bb) {
					if (input[3]) {
						int n = atoi (input + 3);
						bb->folded = n;
					} else {
						bb->folded = !bb->folded;
					}
				} else {
					r_warn_if_reached ();
				}
			}
			}
			break;
		case 0:
		case ' ': // "afb "
		case 'q': // "afbq"
		case 'r': // "afbr"
		case '=': // "afb="
		case '*': // "afb*"
		case 'j': // "afbj"
		case 't': // "afbt"
			anal_fcn_list_bb (core, input + 2, false);
			break;
		case 'i': // "afbi"
			anal_fcn_list_bb (core, input + 2, true);
			break;
		case '.': // "afb."
			anal_fcn_list_bb (core, input[2]? " $$": input + 2, true);
			break;
		case '+': // "afb+"
			anal_fcn_add_bb (core, input + 3);
			break;
		case 'c': // "afbc"
			{
			const char *ptr = input + 3;
			ut64 addr = r_num_math (core->num, ptr);
			ut32 color;
			ptr = strchr (ptr, ' ');
			if (ptr) {
				ptr = strchr (ptr + 1, ' ');
				if (ptr) {
					color = r_num_math (core->num, ptr + 1);
					RAnalOp *op = r_core_op_anal (core, addr);
					if (op) {
						r_anal_colorize_bb (core->anal, addr, color);
						r_anal_op_free (op);
					} else {
						eprintf ("Cannot analyze opcode at 0x%08" PFMT64x "\n", addr);
					}
				}
			}
			}
			break;
		default:
		case '?':
			r_core_cmd_help (core, help_msg_afb);
			break;
		}
		break;
	case 'n': // "afn"
		switch (input[2]) {
		case 's': // "afns"
			if (input[3] == 'j') { // "afnsj"
				free (r_core_anal_fcn_autoname (core, core->offset, 1, input[3]));
			} else {
				free (r_core_anal_fcn_autoname (core, core->offset, 1, 0));
			}
			break;
		case 'a': // "afna"
			{
			char *name = r_core_anal_fcn_autoname (core, core->offset, 0, 0);
			if (name) {
				r_cons_printf ("afn %s 0x%08" PFMT64x "\n", name, core->offset);
				free (name);
			}
			}
			break;
		case '.': // "afn."
		case 0: // "afn"
			{
				RAnalFunction *fcn = r_anal_get_fcn_in (core->anal, core->offset, -1);
				if (fcn) {
					r_cons_printf ("%s\n", fcn->name);
				}
			}
			break;
		case ' ': // "afn "
			{
			ut64 off = core->offset;
			char *p, *name = strdup (r_str_trim_head_ro (input + 3));
			if ((p = strchr (name, ' '))) {
				*p++ = 0;
				off = r_num_math (core->num, p);
			}
			if (*name == '?') {
				eprintf ("Usage: afn newname [off]   # set new name to given function\n");
			} else {
				if (r_str_startswith (name, "base64:")) {
					char *res = (char *)r_base64_decode_dyn (name + 7, -1);
					if (res) {
						free (name);
						name = res;
					}
				}
				if (!*name || !__setFunctionName (core, off, name, false)) {
					eprintf ("Cannot find function at 0x%08" PFMT64x "\n", off);
				}
			}
			free (name);
			}
			break;
		default:
			r_core_cmd_help (core, help_msg_afn);
			break;
		}
		break;
	case 'S': // afS"
		{
		RAnalFunction *fcn = r_anal_get_fcn_in (core->anal, core->offset, -1);
		if (fcn) {
			fcn->maxstack = r_num_math (core->num, input + 3);
			//fcn->stack = fcn->maxstack;
		}
		}
		break;
#if 0
	/* this is undocumented and probably have no uses. plz discuss */
	case 'e': // "afe"
		{
		RAnalFunction *fcn;
		ut64 off = core->offset;
		char *p, *name = strdup ((input[2]&&input[3])? input + 3: "");
		if ((p = strchr (name, ' '))) {
			*p = 0;
			off = r_num_math (core->num, p + 1);
		}
		fcn = r_anal_get_fcn_in (core->anal, off, R_ANAL_FCN_TYPE_FCN | R_ANAL_FCN_TYPE_SYM);
		if (fcn) {
			RAnalBlock *b;
			RListIter *iter;
			RAnalRef *r;
			r_list_foreach (fcn->refs, iter, r) {
				r_cons_printf ("0x%08" PFMT64x " -%c 0x%08" PFMT64x "\n", r->at, r->type, r->addr);
			}
			r_list_foreach (fcn->bbs, iter, b) {
				int ok = 0;
				if (b->type == R_ANAL_BB_TYPE_LAST) ok = 1;
				if (b->type == R_ANAL_BB_TYPE_FOOT) ok = 1;
				if (b->jump == UT64_MAX && b->fail == UT64_MAX) ok = 1;
				if (ok) {
					r_cons_printf ("0x%08" PFMT64x " -r\n", b->addr);
					// TODO: check if destination is outside the function boundaries
				}
			}
		} else eprintf ("Cannot find function at 0x%08" PFMT64x "\n", core->offset);
		free (name);
		}
		break;
#endif
	case 'x': // "afx"
		switch (input[2]) {
		case '\0': // "afx"
		case 'j': // "afxj"
		case ' ': // "afx "
		{
			PJ *pj = pj_new ();
			if (input[2] == 'j') {
				pj_a (pj);
			}
			if (!pj) {
				return false;
			}
			// list xrefs from current address
			{
				ut64 addr = input[2]==' '? r_num_math (core->num, input + 2): core->offset;
				RAnalFunction *fcn = r_anal_get_fcn_in (core->anal, addr, R_ANAL_FCN_TYPE_NULL);
				if (fcn) {
					RAnalRef *ref;
					RListIter *iter;
					RList *refs = r_anal_fcn_get_refs (core->anal, fcn);
					r_list_foreach (refs, iter, ref) {
						if (input[2] == 'j') {
							pj_o (pj);
							pj_ks (pj, "type", r_anal_ref_type_tostring (ref->type));
							pj_kn (pj, "from", ref->at);
							pj_kn (pj, "to", ref->addr);
							pj_end (pj);
						} else {
							r_cons_printf ("%c 0x%08" PFMT64x " -> 0x%08" PFMT64x "\n",
									ref->type, ref->at, ref->addr);
						}
					}
					r_list_free (refs);
				} else {
					eprintf ("afx: Cannot find function at 0x%08"PFMT64x"\n", addr);
				}
			}
			if (input[2] == 'j') {
				pj_end (pj);
				r_cons_println (pj_string (pj));
			}
			pj_free (pj);
			break;
		}
		default:
			eprintf ("Wrong command. Look at af?\n");
			break;
		}
		break;
	case 'F': // "afF"
		{
			int val = input[2] && r_num_math (core->num, input + 2);
			RAnalFunction *fcn = r_anal_get_fcn_in (core->anal, core->offset, R_ANAL_FCN_TYPE_NULL);
			if (fcn) {
				fcn->folded = input[2]? val: !fcn->folded;
			}
		}
		break;
	case '?': // "af?"
		r_core_cmd_help (core, help_msg_af);
		break;
	case 'r': // "afr" // analyze function recursively
	case ' ': // "af "
	case '\0': // "af"
		{
		char *uaddr = NULL, *name = NULL;
		int depth = r_config_get_i (core->config, "anal.depth");
		bool analyze_recursively = r_config_get_i (core->config, "anal.calls");
		RAnalFunction *fcn = NULL;
		ut64 addr = core->offset;
		if (input[1] == 'r') {
			input++;
			analyze_recursively = true;
		}

		// first undefine
		if (input[0] && input[1] == ' ') {
			name = strdup (r_str_trim_head_ro (input + 2));
			uaddr = strchr (name, ' ');
			if (uaddr) {
				*uaddr++ = 0;
				addr = r_num_math (core->num, uaddr);
			}
			// depth = 1; // or 1?
			// disable hasnext
		}
		//r_core_anal_undefine (core, core->offset);
		r_core_anal_fcn (core, addr, UT64_MAX, R_ANAL_REF_TYPE_NULL, depth);
		fcn = r_anal_get_fcn_in (core->anal, addr, 0);
		if (fcn) {
			/* ensure we use a proper name */
			__setFunctionName (core, addr, fcn->name, false);
			if (core->anal->opt.vars) {
				r_core_recover_vars (core, fcn, true);
			}
			__add_vars_sdb (core, fcn);
		} else {
			if (core->anal->verbose) {
				eprintf ("Warning: Unable to analyze function at 0x%08"PFMT64x"\n", addr);
			}
		}
		if (analyze_recursively) {
			fcn = r_anal_get_fcn_in (core->anal, addr, 0); /// XXX wrong in case of nopskip
			if (fcn) {
				RAnalRef *ref;
				RListIter *iter;
				RList *refs = r_anal_fcn_get_refs (core->anal, fcn);
				r_list_foreach (refs, iter, ref) {
					if (ref->addr == UT64_MAX) {
						//eprintf ("Warning: ignore 0x%08"PFMT64x" call 0x%08"PFMT64x"\n", ref->at, ref->addr);
						continue;
					}
					if (ref->type != R_ANAL_REF_TYPE_CODE && ref->type != R_ANAL_REF_TYPE_CALL) {
						/* only follow code/call references */
						continue;
					}
					if (!r_io_is_valid_offset (core->io, ref->addr, !core->anal->opt.noncode)) {
						continue;
					}
					r_core_anal_fcn (core, ref->addr, fcn->addr, R_ANAL_REF_TYPE_CALL, depth);
					/* use recursivity here */
#if 1
					RAnalFunction *f = r_anal_get_function_at (core->anal, ref->addr);
					if (f) {
						RListIter *iter;
						RAnalRef *ref;
						RList *refs1 = r_anal_fcn_get_refs (core->anal, f);
						r_list_foreach (refs1, iter, ref) {
							if (!r_io_is_valid_offset (core->io, ref->addr, !core->anal->opt.noncode)) {
								continue;
							}
							if (ref->type != 'c' && ref->type != 'C') {
								continue;
							}
							r_core_anal_fcn (core, ref->addr, f->addr, R_ANAL_REF_TYPE_CALL, depth);
							// recursively follow fcn->refs again and again
						}
						r_list_free (refs1);
					} else {
						f = r_anal_get_fcn_in (core->anal, fcn->addr, 0);
						if (f) {
							/* cut function */
							r_anal_fcn_resize (core->anal, f, addr - fcn->addr);
							r_core_anal_fcn (core, ref->addr, fcn->addr,
									R_ANAL_REF_TYPE_CALL, depth);
							f = r_anal_get_function_at (core->anal, fcn->addr);
						}
						if (!f) {
							eprintf ("af: Cannot find function at 0x%08" PFMT64x "\n", fcn->addr);
						}
					}
#endif
				}
				r_list_free (refs);
				if (core->anal->opt.vars) {
					r_core_recover_vars (core, fcn, true);
				}
			}
		}
		if (name) {
			if (*name && !__setFunctionName (core, addr, name, true)) {
				eprintf ("af: Cannot find function at 0x%08" PFMT64x "\n", addr);
			}
			free (name);
		}
		r_core_anal_propagate_noreturn (core);
#if 0
		// XXX THIS IS VERY SLOW
		if (core->anal->opt.vars) {
			RListIter *iter;
			RAnalFunction *fcni = NULL;
			r_list_foreach (core->anal->fcns, iter, fcni) {
				if (r_cons_is_breaked ()) {
					break;
				}
				r_core_recover_vars (core, fcni, true);
			}
		}
#endif
		flag_every_function (core);
	}
		break;
	default:
		return false;
		break;
	}
	return true;
}