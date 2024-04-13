static int core_anal_graph_construct_edges(RCore *core, RAnalFunction *fcn, int opts, PJ *pj, Sdb *DB) {
	RAnalBlock *bbi;
	RListIter *iter;
	int is_keva = opts & R_CORE_ANAL_KEYVALUE;
	int is_star = opts & R_CORE_ANAL_STAR;
	int is_json = opts & R_CORE_ANAL_JSON;
	int is_html = r_cons_context ()->is_html;
	char *pal_jump = palColorFor ("graph.true");
	char *pal_fail = palColorFor ("graph.false");
	char *pal_trfa = palColorFor ("graph.trufae");
	int nodes = 0;
	r_list_foreach (fcn->bbs, iter, bbi) {
		if (bbi->jump != UT64_MAX) {
			nodes++;
			if (is_keva) {
				char key[128];
				char val[128];
				snprintf (key, sizeof (key), "bb.0x%08"PFMT64x".to", bbi->addr);
				if (bbi->fail != UT64_MAX) {
					snprintf (val, sizeof (val), "0x%08"PFMT64x, bbi->jump);
				} else {
					snprintf (val, sizeof (val), "0x%08"PFMT64x ",0x%08"PFMT64x,
							bbi->jump, bbi->fail);
				}
				// bb.<addr>.to=<jump>,<fail>
				sdb_set (DB, key, val, 0);
			} else if (is_html) {
				r_cons_printf ("<div class=\"connector _0x%08"PFMT64x" _0x%08"PFMT64x"\">\n"
						"  <img class=\"connector-end\" src=\"img/arrow.gif\" /></div>\n",
						bbi->addr, bbi->jump);
			} else if (!is_json && !is_keva) {
				if (is_star) {
					char *from = get_title (bbi->addr);
					char *to = get_title (bbi->jump);
					r_cons_printf ("age %s %s\n", from, to);
					free (from);
					free (to);
				} else {
					r_strf_buffer (128);
					const char* edge_color = bbi->fail != -1 ? pal_jump : pal_trfa;
					if (sdb_const_get (core->sdb, r_strf ("agraph.edge.0x%"PFMT64x"_0x%"PFMT64x".highlight", bbi->addr, bbi->jump), 0)) {
						edge_color = "cyan";
					}
					r_cons_printf ("        \"0x%08"PFMT64x"\" -> \"0x%08"PFMT64x"\" "
							"[color=\"%s\"];\n", bbi->addr, bbi->jump, edge_color);
					core_anal_color_curr_node (core, bbi);
				}
			}
		}
		if (bbi->fail != -1) {
			nodes++;
			if (is_html) {
				r_cons_printf ("<div class=\"connector _0x%08"PFMT64x" _0x%08"PFMT64x"\">\n"
						"  <img class=\"connector-end\" src=\"img/arrow.gif\"/></div>\n",
						bbi->addr, bbi->fail);
			} else if (!is_keva && !is_json) {
				if (is_star) {
					char *from = get_title (bbi->addr);
					char *to = get_title (bbi->fail);
					r_cons_printf ("age %s %s\n", from, to);
					free(from);
					free(to);
				} else {
					r_cons_printf ("        \"0x%08"PFMT64x"\" -> \"0x%08"PFMT64x"\" "
									"[color=\"%s\"];\n", bbi->addr, bbi->fail, pal_fail);
					core_anal_color_curr_node (core, bbi);
				}
			}
		}
		if (bbi->switch_op) {
			RAnalCaseOp *caseop;
			RListIter *iter;

			if (bbi->fail != UT64_MAX) {
				if (is_html) {
					r_cons_printf ("<div class=\"connector _0x%08"PFMT64x" _0x%08"PFMT64x"\">\n"
							"  <img class=\"connector-end\" src=\"img/arrow.gif\"/></div>\n",
							bbi->addr, bbi->fail);
				} else if (!is_keva && !is_json) {
					if (is_star) {
						char *from = get_title (bbi->addr);
						char *to = get_title (bbi->fail);
						r_cons_printf ("age %s %s\n", from, to);
						free(from);
						free(to);
					} else {
						r_cons_printf ("        \"0x%08"PFMT64x"\" -> \"0x%08"PFMT64x"\" "
								"[color=\"%s\"];\n", bbi->addr, bbi->fail, pal_fail);
						core_anal_color_curr_node (core, bbi);
					}
				}
			}
			r_list_foreach (bbi->switch_op->cases, iter, caseop) {
				nodes++;
				if (is_keva) {
					char key[128];
					snprintf (key, sizeof (key),
							"bb.0x%08"PFMT64x".switch.%"PFMT64d,
							bbi->addr, caseop->value);
					sdb_num_set (DB, key, caseop->jump, 0);
					snprintf (key, sizeof (key),
							"bb.0x%08"PFMT64x".switch", bbi->addr);
							sdb_array_add_num (DB, key, caseop->value, 0);
				} else if (is_html) {
					r_cons_printf ("<div class=\"connector _0x%08" PFMT64x " _0x%08" PFMT64x "\">\n"
							"  <img class=\"connector-end\" src=\"img/arrow.gif\"/></div>\n",
							bbi->addr, caseop->addr);
				} else if (!is_json && !is_keva) {
					if (is_star) {
						char *from = get_title (bbi->addr);
						char *to = get_title (caseop->addr);
						r_cons_printf ("age %s %s\n", from ,to);
						free (from);
						free (to);
					} else {
						r_cons_printf ("        \"0x%08" PFMT64x "\" -> \"0x%08" PFMT64x "\" "
								"[color=\"%s\"];\n",
								bbi->addr, caseop->addr, pal_trfa);
						core_anal_color_curr_node (core, bbi);
					}
				}
			}
		}
	}
	free(pal_jump);
	free(pal_fail);
	free(pal_trfa);
	return nodes;
}