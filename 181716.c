static bool anal_fcn_list_bb(RCore *core, const char *input, bool one) {
	RDebugTracepoint *tp = NULL;
	RListIter *iter;
	RAnalBlock *b;
	int mode = 0;
	ut64 addr, bbaddr = UT64_MAX;
	PJ *pj = NULL;

	if (*input == '.') {
		one = true;
		input++;
	}
	if (*input) {
		mode = *input;
		input++;
	}
	if (*input == '.') {
		one = true;
		input++;
	}
	if (input && *input) {
		addr = bbaddr = r_num_math (core->num, input);
		if (!addr && *input != '0') {
			addr = core->offset;
		}
	} else {
		addr = core->offset;
	}
	input = r_str_trim_head_ro (input);
	if (one) {
		bbaddr = addr;
	}
	if (mode == 'j') {
		pj = pj_new ();
		if (!pj) {
			return false;
		}
		pj_a (pj);
	}
	RAnalFunction *fcn = r_anal_get_fcn_in (core->anal, addr, 0);
	if (!fcn) {
		if (mode == 'j') {
			pj_end (pj);
			r_cons_println (pj_string (pj));
			pj_free (pj);
		}
		eprintf ("Cannot find function in 0x%08"PFMT64x"\n", addr);
		return false;
	}
	if (mode == '*') {
		r_cons_printf ("fs blocks\n");
	}
	if (fcn->bbs) {
		r_list_sort (fcn->bbs, bb_cmp);
	}
	if (mode == '=') { // afb
		RList *flist = r_list_newf ((RListFree) r_listinfo_free);
		if (!flist) {
			return false;
		}
		ls_foreach (fcn->bbs, iter, b) {
			RInterval inter = (RInterval) {b->addr, b->size};
			RListInfo *info = r_listinfo_new (NULL, inter, inter, -1, NULL);
			if (!info) {
				break;
			}
			r_list_append (flist, info);
		}
		RTable *table = r_core_table (core);
		r_table_visual_list (table, flist, core->offset, core->blocksize,
			r_cons_get_size (NULL), r_config_get_i (core->config, "scr.color"));
		r_cons_printf ("\n%s\n", r_table_tostring (table));
		r_table_free (table);
		r_list_free (flist);
		return true;
	}

	RTable *t = NULL;
	if (mode == 't') {
		t = r_table_new ();
		r_table_set_columnsf (t, "xdxx", "addr", "size", "jump", "fail");
	}
	if (fcn->bbs) {
		r_list_foreach (fcn->bbs, iter, b) {
			if (one) {
				if (bbaddr != UT64_MAX && (bbaddr < b->addr || bbaddr >= (b->addr + b->size))) {
					continue;
				}
			}
			switch (mode) {
			case 't':
				r_table_add_rowf (t, "xdxx", b->addr, b->size, b->jump, b->fail);
				break;
			case 'r':
				if (b->jump == UT64_MAX) {
					ut64 retaddr = r_anal_bb_opaddr_i (b, b->ninstr - 1);
					if (retaddr == UT64_MAX) {
						break;
					}

					if (!strcmp (input, "*")) {
						r_cons_printf ("db 0x%08"PFMT64x"\n", retaddr);
					} else if (!strcmp (input, "-*")) {
						r_cons_printf ("db-0x%08"PFMT64x"\n", retaddr);
					} else {
						r_cons_printf ("0x%08"PFMT64x"\n", retaddr);
					}
				}
				break;
			case '*':
				r_cons_printf ("f bb.%05" PFMT64x " = 0x%08" PFMT64x "\n",
					b->addr & 0xFFFFF, b->addr);
				break;
			case 'q':
				r_cons_printf ("0x%08" PFMT64x "\n", b->addr);
				break;
			case 'j':
				//r_cons_printf ("%" PFMT64u "%s", b->addr, iter->n? ",": "");
				{
				RListIter *iter2;
				RAnalBlock *b2;
				int inputs = 0;
				int outputs = 0;
				r_list_foreach (fcn->bbs, iter2, b2) {
					if (b2->jump == b->addr) {
						inputs++;
					}
					if (b2->fail == b->addr) {
						inputs++;
					}
				}
				if (b->jump != UT64_MAX) {
					outputs ++;
				}
				if (b->fail != UT64_MAX) {
					outputs ++;
				}
				pj_o (pj);

				if (b->jump != UT64_MAX) {
					pj_kn (pj, "jump", b->jump);
				}
				if (b->fail != UT64_MAX) {
					pj_kn (pj, "fail", b->fail);
				}
				if (b->switch_op) {
					pj_k (pj, "switch_op");
					pj_o (pj);
					pj_kn (pj, "addr", b->switch_op->addr);
					pj_kn (pj, "min_val", b->switch_op->min_val);
					pj_kn (pj, "def_val", b->switch_op->def_val);
					pj_kn (pj, "max_val", b->switch_op->max_val);
					pj_k (pj, "cases");
					pj_a (pj);
					{
						RListIter *case_op_iter;
						RAnalCaseOp *case_op;
						r_list_foreach (b->switch_op->cases, case_op_iter, case_op) {
							pj_o (pj);
							pj_kn (pj, "addr", case_op->addr);
							pj_kn (pj, "jump", case_op->jump);
							pj_kn (pj, "value", case_op->value);
							pj_end (pj);
						}
					}
					pj_end (pj);
					pj_end (pj);
				}
				pj_kn (pj, "addr", b->addr);
				pj_ki (pj, "size", b->size);
				pj_ki (pj, "inputs", inputs);
				pj_ki (pj, "outputs", outputs);
				pj_ki (pj, "ninstr", b->ninstr);
				pj_kb (pj, "traced", b->traced);
				pj_end (pj);
				}
				break;
			case 'i':
				{
				RListIter *iter2;
				RAnalBlock *b2;
				int inputs = 0;
				int outputs = 0;
				r_list_foreach (fcn->bbs, iter2, b2) {
					if (b2->jump == b->addr) {
						inputs++;
					}
					if (b2->fail == b->addr) {
						inputs++;
					}
				}
				if (b->jump != UT64_MAX) {
					outputs ++;
				}
				if (b->fail != UT64_MAX) {
					outputs ++;
				}
				if (b->switch_op) {
					RList *unique_cases = r_list_uniq (b->switch_op->cases, casecmp);
					outputs += r_list_length (unique_cases);
					r_list_free (unique_cases);
				}
				if (b->jump != UT64_MAX) {
					r_cons_printf ("jump: 0x%08"PFMT64x"\n", b->jump);
				}
				if (b->fail != UT64_MAX) {
					r_cons_printf ("fail: 0x%08"PFMT64x"\n", b->fail);
				}
				r_cons_printf ("addr: 0x%08"PFMT64x"\nsize: %d\ninputs: %d\noutputs: %d\nninstr: %d\ntraced: %s\n",
					b->addr, b->size, inputs, outputs, b->ninstr, r_str_bool (b->traced));
				}
				break;
			default:
				tp = r_debug_trace_get (core->dbg, b->addr);
				r_cons_printf ("0x%08" PFMT64x " 0x%08" PFMT64x " %02X:%04X %d",
					b->addr, b->addr + b->size,
					tp? tp->times: 0, tp? tp->count: 0,
					b->size);
				if (b->jump != UT64_MAX) {
					r_cons_printf (" j 0x%08" PFMT64x, b->jump);
				}
				if (b->fail != UT64_MAX) {
					r_cons_printf (" f 0x%08" PFMT64x, b->fail);
				}
				if (b->switch_op) {
					RAnalCaseOp *cop;
					RListIter *iter;
					RList *unique_cases = r_list_uniq (b->switch_op->cases, casecmp);
					r_list_foreach (unique_cases, iter, cop) {
						r_cons_printf (" s 0x%08" PFMT64x, cop->addr);
					}
					r_list_free (unique_cases);
				}
				r_cons_newline ();
				break;
			}
		}
	}
	if (mode == 't') {
		const char *arg = input;
		if (r_table_query (t, arg)) {
			char *ts = r_table_tofancystring (t);
			r_cons_printf ("%s", ts);
			free (ts);
		}
		r_table_free (t);
	} else if (mode == 'j') {
		pj_end (pj);
		r_cons_println (pj_string (pj));
		pj_free (pj);
	}
	return true;
}