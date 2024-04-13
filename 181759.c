static bool cmd_anal_refs(RCore *core, const char *input) {
	ut64 addr = core->offset;
	switch (input[0]) {
	case '-': { // "ax-"
		RList *list;
		RListIter *iter;
		RAnalRef *ref;
		char *cp_inp = strdup (input + 1);
		char *ptr = cp_inp;
		r_str_trim_head (ptr);
		if (!strcmp (ptr, "*")) { // "ax-*"
			r_anal_xrefs_init (core->anal);
		} else {
			int n = r_str_word_set0 (ptr);
			ut64 from = UT64_MAX, to = UT64_MAX;
			switch (n) {
			case 2:
				from = r_num_math (core->num, r_str_word_get0 (ptr, 1));
				//fall through
			case 1: // get addr
				to = r_num_math (core->num, r_str_word_get0 (ptr, 0));
				break;
			default:
				to = core->offset;
				break;
			}
			list = r_anal_xrefs_get (core->anal, to);
			if (list) {
				r_list_foreach (list, iter, ref) {
					if (from != UT64_MAX && from == ref->addr) {
						r_anal_xref_del (core->anal, ref->addr, ref->at);
					}
					if (from == UT64_MAX) {
						r_anal_xref_del (core->anal, ref->addr, ref->at);
					}
				}
			}
			r_list_free (list);
		}
		free (cp_inp);
	} break;
	case 'g': // "axg"
		{
			Sdb *db = sdb_new0 ();
			if (input[1] == '*') {
				anal_axg (core, input + 2, 0, db, R_CORE_ANAL_GRAPHBODY, NULL); // r2 commands
			} else if (input[1] == 'j') {
				PJ *pj = pj_new ();
				anal_axg (core, input + 2, 0, db, R_CORE_ANAL_JSON, pj);
				r_cons_printf("%s\n", pj_string (pj));
				pj_free (pj);
			} else {
				anal_axg (core, input[1] ? input + 2 : NULL, 0, db, 0, NULL);
			}
			sdb_free (db);
		}
		break;
	case '\0': // "ax"
	case 'j': // "axj"
	case 'q': // "axq"
	case '*': // "ax*"
		r_anal_xrefs_list (core->anal, input[0]);
		break;
	case '.': { // "ax."
		char *tInput = strdup (input);
		if (r_str_replace_ch (tInput, '.', 't', false)) {
			cmd_anal_refs (core, tInput);
		}
		char *fInput = strdup (input);
		if (r_str_replace_ch (fInput, '.', 'f', false)) {
			cmd_anal_refs (core, fInput);
		}
		free (tInput);
		free (fInput);
	} break;
	case 'm': { // "axm"
		RList *list;
		RAnalRef *ref;
		RListIter *iter;
		char *ptr = strdup (r_str_trim_head_ro (input + 1));
		int n = r_str_word_set0 (ptr);
		ut64 at = core->offset;
		ut64 addr = UT64_MAX;
		switch (n) {
		case 2: // get at
			at = r_num_math (core->num, r_str_word_get0 (ptr, 1));
		/* fall through */
		case 1: // get addr
			addr = r_num_math (core->num, r_str_word_get0 (ptr, 0));
			break;
		default:
			free (ptr);
			return false;
		}
		//get all xrefs pointing to addr
		list = r_anal_xrefs_get (core->anal, addr);
		r_list_foreach (list, iter, ref) {
			r_cons_printf ("0x%"PFMT64x" %s\n", ref->addr, r_anal_xrefs_type_tostring (ref->type));
			r_anal_xrefs_set (core->anal, ref->addr, at, ref->type);
		}
		r_list_free (list);
		free (ptr);
	} break;
	case 't': { // "axt"
		RList *list = NULL;
		RAnalFunction *fcn;
		RAnalRef *ref;
		RListIter *iter;
		char *space = strchr (input, ' ');
		char *tmp = NULL;
		char *name = space ? strdup (space + 1): NULL;

		if (name && (tmp = strchr (name, ' '))) {
			char *varname = tmp + 1;
			*tmp = '\0';
			RAnalFunction *fcn = r_anal_fcn_find_name (core->anal, name);
			if (fcn) {
				RAnalVar *var = r_anal_var_get_byname (core->anal, fcn->addr, varname);
				if (var) {
					const char *rvar = var_ref_list (fcn->addr, var->delta, 'R');
					const char *wvar = var_ref_list (fcn->addr, var->delta, 'W');
					char *res = sdb_get (core->anal->sdb_fcns, rvar, 0);
					char *res1 = sdb_get (core->anal->sdb_fcns, wvar, 0);
					const char *ref;
					RListIter *iter;
					RList *list = (res && *res)? r_str_split_list (res, ",", 0): NULL;
					RList *list1 = (res1 && *res1)? r_str_split_list (res1, ",", 0): NULL;
					r_list_join (list , list1);
					r_list_foreach (list, iter, ref) {
						ut64 addr = r_num_math (NULL, ref);
						char *op = get_buf_asm (core, core->offset, addr, fcn, true);
						r_cons_printf ("%s 0x%"PFMT64x" [DATA] %s\n", fcn?  fcn->name : "(nofunc)", addr, op);
						free (op);
					}
					free (res);
					free (res1);
					R_FREE (name);
					r_anal_var_free (var);
					r_list_free (list);
					r_list_free (list1);
					break;
				}
			}
		}
		if (space) {
			addr = r_num_math (core->num, space + 1);
		} else {
			addr = core->offset;
		}
		list = r_anal_xrefs_get (core->anal, addr);
		if (list) {
			if (input[1] == 'q') { // "axtq"
				r_list_foreach (list, iter, ref) {
					r_cons_printf ("0x%" PFMT64x "\n", ref->addr);
				}
			} else if (input[1] == 'j') { // "axtj"
				PJ *pj = pj_new ();
				if (!pj) {
					return false;
				}
				pj_a (pj);
				r_list_foreach (list, iter, ref) {
					fcn = r_anal_get_fcn_in (core->anal, ref->addr, 0);
					char *str = get_buf_asm (core, addr, ref->addr, fcn, false);
					pj_o (pj);
					pj_kn (pj, "from", ref->addr);
					pj_ks (pj, "type", r_anal_xrefs_type_tostring (ref->type));
					pj_ks (pj, "opcode", str);
					if (fcn) {
						pj_kn (pj, "fcn_addr", fcn->addr);
						pj_ks (pj, "fcn_name", fcn->name);
					}
					RFlagItem *fi = r_flag_get_at (core->flags, fcn? fcn->addr: ref->addr, true);
					if (fi) {
						if (fcn) {
							if (strcmp (fcn->name, fi->name)) {
								pj_ks (pj, "flag", fi->name);
							}
						} else {
							pj_k (pj, "name");
							if (fi->offset != ref->addr) {
								int delta = (int)(ref->addr - fi->offset);
								char *name_ref = r_str_newf ("%s+%d", fi->name, delta);
								pj_s (pj, name_ref);
								free (name_ref);
							} else {
								pj_s (pj, fi->name);
							}
						}
						if (fi->realname && strcmp (fi->name, fi->realname)) {
							char *escaped = r_str_escape (fi->realname);
							if (escaped) {
								pj_ks (pj, "realname", escaped);
								free (escaped);
							}
						}
					}
					char *refname = core->anal->coreb.getNameDelta (core, ref->at);
					if (refname) {
						r_str_replace_ch (refname, ' ', 0, true);
						pj_ks (pj, "refname", refname);
						free (refname);
					}
					pj_end (pj);
					free (str);
				}
				pj_end (pj);
				r_cons_printf ("%s", pj_string (pj));
				pj_free (pj);
				r_cons_newline ();
			} else if (input[1] == 'g') { // axtg
				r_list_foreach (list, iter, ref) {
					char *str = r_core_cmd_strf (core, "fd 0x%"PFMT64x, ref->addr);
					if (!str) {
						str = strdup ("?\n");
					}
					r_str_trim_tail (str);
					r_cons_printf ("agn 0x%" PFMT64x " \"%s\"\n", ref->addr, str);
					free (str);
				}
				if (input[2] != '*') {
					RAnalFunction *fcn = r_anal_get_fcn_in (core->anal, addr, 0);
					r_cons_printf ("agn 0x%" PFMT64x " \"%s\"\n", addr, fcn?fcn->name: "$$");
				}
				r_list_foreach (list, iter, ref) {
					r_cons_printf ("age 0x%" PFMT64x " 0x%"PFMT64x"\n", ref->addr, addr);
				}
			} else if (input[1] == '*') { // axt*
				// TODO: implement multi-line comments
				r_list_foreach (list, iter, ref)
					r_cons_printf ("CCa 0x%" PFMT64x " \"XREF type %d at 0x%" PFMT64x"%s\n",
						ref->addr, ref->type, addr, iter->n? ",": "");
			} else { // axt
				RAnalFunction *fcn;
				char *comment;
				r_list_foreach (list, iter, ref) {
					fcn = r_anal_get_fcn_in (core->anal, ref->addr, 0);
					char *buf_asm = get_buf_asm (core, addr, ref->addr, fcn, true);
					comment = r_meta_get_string (core->anal, R_META_TYPE_COMMENT, ref->addr);
					char *buf_fcn = comment
						? r_str_newf ("%s; %s", fcn ?  fcn->name : "(nofunc)", strtok (comment, "\n"))
						: r_str_newf ("%s", fcn ? fcn->name : "(nofunc)");
					r_cons_printf ("%s 0x%" PFMT64x " [%s] %s\n",
						buf_fcn, ref->addr, r_anal_xrefs_type_tostring (ref->type), buf_asm);
					free (buf_asm);
					free (buf_fcn);
				}
			}
		} else {
			if (input[1] == 'j') { // "axtj"
				PJ *pj = pj_new ();
				if (!pj) {
					return false;
				}
				pj_a (pj);
				pj_end (pj);
				r_cons_println (pj_string (pj));
				pj_free (pj);
			}
		}
		r_list_free (list);
	} break;
	case 'f':
		if (input[1] == 'f') { // "axff"
			RAnalFunction * fcn = r_anal_get_fcn_in (core->anal, addr, 0);
			RListIter *iter;
			PJ *pj = NULL;
			RAnalRef *refi;
			if (input[2] == 'j') { // "axffj"
				// start a new JSON object
				pj = pj_new ();
				pj_a (pj);
			}
			if (fcn) {
				RList *refs = r_anal_fcn_get_refs (core->anal, fcn);
				r_list_foreach (refs, iter, refi) {
					RFlagItem *f = r_flag_get_at (core->flags, refi->addr, true);
					const char *name = f ? f->name: "";
					if (input[2] == 'j') {
						pj_o (pj);
						pj_ks (pj, "type", r_anal_xrefs_type_tostring(refi->type));
						pj_kn (pj, "at", refi->at);
						pj_kn (pj, "ref", refi->addr);
						pj_ks (pj, "name", name);
						pj_end (pj);
					} else {
						r_cons_printf ("%s 0x%08"PFMT64x" 0x%08"PFMT64x" %s\n",
							r_anal_xrefs_type_tostring(refi->type), refi->at, refi->addr, name);
					}
				}
				if (input[2] ==  'j') {
					pj_end (pj);
				}
				if (pj) {
					r_cons_println (pj_string (pj));
					pj_free (pj);
				}
			} else {
				eprintf ("Cannot find any function\n");
			}
		} else { // "axf"
			RAsmOp asmop;
			RList *list, *list_ = NULL;
			RAnalRef *ref;
			RListIter *iter;
			char *space = strchr (input, ' ');
			if (space) {
				addr = r_num_math (core->num, space + 1);
			} else {
				addr = core->offset;
			}
			RAnalFunction * fcn = r_anal_get_fcn_in (core->anal, addr, 0);
			if (input[1] == '.') { // "axf."
				list = list_ = r_anal_xrefs_get_from (core->anal, addr);
				if (!list) {
					list = r_anal_fcn_get_refs (core->anal, fcn);
				}
			} else {
				list = r_anal_refs_get (core->anal, addr);
			}

			if (list) {
				if (input[1] == 'q') { // "axfq"
					r_list_foreach (list, iter, ref) {
						r_cons_printf ("0x%" PFMT64x "\n", ref->at);
					}
				} else if (input[1] == 'j') { // "axfj"
					PJ *pj = pj_new ();
					if (!pj) {
						return false;
					}
					pj_a (pj);
					r_list_foreach (list, iter, ref) {
						// TODO: Use r_core_anal_op(DISASM) instead of all those 4 lines
						ut8 buf[16];
						r_io_read_at (core->io, ref->addr, buf, sizeof (buf));
						r_asm_set_pc (core->assembler, ref->addr);
						r_asm_disassemble (core->assembler, &asmop, buf, sizeof (buf));
						pj_o (pj);
						pj_kn (pj, "from", ref->at);
						pj_kn (pj, "to", ref->addr);
						pj_ks (pj, "type", r_anal_xrefs_type_tostring (ref->type));
						pj_ks (pj, "opcode", r_asm_op_get_asm (&asmop));
						pj_end (pj);
					}
					pj_end (pj);
					r_cons_println (pj_string (pj));
					pj_free (pj);
				} else if (input[1] == '*') { // "axf*"
					// TODO: implement multi-line comments
					r_list_foreach (list, iter, ref) {
						r_cons_printf ("CCa 0x%" PFMT64x " \"XREF from 0x%" PFMT64x "\n",
								ref->at, ref->type, r_asm_op_get_asm (&asmop), iter->n? ",": "");
					}
				} else { // "axf"
					char str[512];
					int has_color = core->print->flags & R_PRINT_FLAGS_COLOR;
					r_list_foreach (list, iter, ref) {
						ut8 buf[16];
						char *desc;
						char *desc_to_free = NULL;
						RFlagItem *flag = r_flag_get_at (core->flags, ref->addr, false);
						if (flag) {
							desc = flag->name;
						} else {
							r_io_read_at (core->io, ref->addr, buf, sizeof (buf));
							r_asm_set_pc (core->assembler, ref->addr);
							r_asm_disassemble (core->assembler, &asmop, buf, sizeof(buf));
							RAnalHint *hint = r_anal_hint_get (core->anal, ref->addr);
							r_parse_filter (core->parser, ref->addr, core->flags, hint, r_asm_op_get_asm (&asmop),
									str, sizeof (str), core->print->big_endian);
							r_anal_hint_free (hint);
							if (has_color) {
								desc = desc_to_free = r_print_colorize_opcode (core->print, str,
										core->cons->context->pal.reg, core->cons->context->pal.num, false, fcn ? fcn->addr : 0);
							} else {
								desc = str;
							}
						}
						r_cons_printf ("%c 0x%" PFMT64x " %s",
								ref->type ? ref->type : ' ', ref->addr, desc);

						if (ref->type == R_ANAL_REF_TYPE_CALL) {
							RAnalOp aop;
							r_anal_op (core->anal, &aop, ref->addr, buf, sizeof(buf), R_ANAL_OP_MASK_BASIC);
							if (aop.type == R_ANAL_OP_TYPE_UCALL) {
								cmd_anal_ucall_ref (core, ref->addr);
							}
						}
						r_cons_newline ();
						free (desc_to_free);
					}
				}
			} else {
				if (input[1] == 'j') { // "axfj"
					r_cons_print ("[]\n");
				}
			}
			r_list_free (list);
		}
		break;
	case 'F': // "axF"
		find_refs (core, input + 1);
		break;
	case 'C': // "axC"
	case 'c': // "axc"
	case 'd': // "axd"
	case 's': // "axs"
	case ' ': // "ax "
		{
		char *ptr = strdup (r_str_trim_head_ro ((char *)input + 1));
		int n = r_str_word_set0 (ptr);
		ut64 at = core->offset;
		ut64 addr = UT64_MAX;
		RAnalRefType reftype = r_anal_xrefs_type (input[0]);
		switch (n) {
		case 2: // get at
			at = r_num_math (core->num, r_str_word_get0 (ptr, 1));
		/* fall through */
		case 1: // get addr
			addr = r_num_math (core->num, r_str_word_get0 (ptr, 0));
			break;
		default:
			free (ptr);
			return false;
		}
		r_anal_xrefs_set (core->anal, at, addr, reftype);
		free (ptr);
		}
	   	break;
	default:
	case '?':
		r_core_cmd_help (core, help_msg_ax);
		break;
	}

	return true;
}