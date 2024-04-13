static int core_anal_graph_construct_nodes(RCore *core, RAnalFunction *fcn, int opts, PJ *pj, Sdb *DB) {
	RAnalBlock *bbi;
	RListIter *iter;
	int is_keva = opts & R_CORE_ANAL_KEYVALUE;
	int is_star = opts & R_CORE_ANAL_STAR;
	int is_json = opts & R_CORE_ANAL_JSON;
	int is_html = r_cons_context ()->is_html;
	int left = 300;
	int top = 0;

	int is_json_format_disasm = opts & R_CORE_ANAL_JSON_FORMAT_DISASM;
	char *pal_curr = palColorFor ("graph.current");
	char *pal_traced = palColorFor ("graph.traced");
	char *pal_box4 = palColorFor ("graph.box4");
	const char *font = r_config_get (core->config, "graph.font");
	bool color_current = r_config_get_i (core->config, "graph.gv.current");
	char *str;
	int nodes = 0;
	r_list_foreach (fcn->bbs, iter, bbi) {
		if (is_keva) {
			char key[128];
			sdb_array_push_num (DB, "bbs", bbi->addr, 0);
			snprintf (key, sizeof (key), "bb.0x%08"PFMT64x".size", bbi->addr);
			sdb_num_set (DB, key, bbi->size, 0); // bb.<addr>.size=<num>
		} else if (is_json) {
			RDebugTracepoint *t = r_debug_trace_get (core->dbg, bbi->addr);
			pj_o (pj);
			pj_kn (pj, "offset", bbi->addr);
			pj_kn (pj, "size", bbi->size);
			if (bbi->jump != UT64_MAX) {
				pj_kn (pj, "jump", bbi->jump);
			}
			if (bbi->fail != -1) {
				pj_kn (pj, "fail", bbi->fail);
			}
			if (bbi->switch_op) {
				RAnalSwitchOp *op = bbi->switch_op;
				pj_k (pj, "switchop");
				pj_o (pj);
				pj_kn (pj, "offset", op->addr);
				pj_kn (pj, "defval", op->def_val);
				pj_kn (pj, "maxval", op->max_val);
				pj_kn (pj, "minval", op->min_val);
				pj_k (pj, "cases");
				pj_a (pj);
				RAnalCaseOp *case_op;
				RListIter *case_iter;
				r_list_foreach (op->cases, case_iter, case_op) {
					pj_o (pj);
					pj_kn (pj, "offset", case_op->addr);
					pj_kn (pj, "value", case_op->value);
					pj_kn (pj, "jump", case_op->jump);
					pj_end (pj);
				}
				pj_end (pj);
				pj_end (pj);
			}
			if (t) {
				pj_k (pj, "trace");
				pj_o (pj);
				pj_ki (pj, "count", t->count);
				pj_ki (pj, "times", t->times);
				pj_end (pj);
			}
			if (bbi->color.r || bbi->color.g || bbi->color.b) {
				char *s = r_cons_rgb_tostring (bbi->color.r, bbi->color.g, bbi->color.b);
				pj_ks (pj, "color", s);
				free (s);
			}
			pj_k (pj, "ops");
			pj_a (pj);
			ut8 *buf = malloc (bbi->size);
			if (buf) {
				r_io_read_at (core->io, bbi->addr, buf, bbi->size);
				if (is_json_format_disasm) {
					r_core_print_disasm (core, bbi->addr, buf, bbi->size, bbi->size, 0, NULL, true, true, pj, NULL);
				} else {
					r_core_print_disasm_json (core, bbi->addr, buf, bbi->size, 0, pj);
				}
				free (buf);
			} else {
				eprintf ("cannot allocate %"PFMT64u" byte(s)\n", bbi->size);
			}
			pj_end (pj);
			pj_end (pj);
			continue;
		}
		if ((str = core_anal_graph_label (core, bbi, opts))) {
			if (opts & R_CORE_ANAL_GRAPHDIFF) {
				const char *difftype = bbi->diff? (\
				bbi->diff->type==R_ANAL_DIFF_TYPE_MATCH? "lightgray":
				bbi->diff->type==R_ANAL_DIFF_TYPE_UNMATCH? "yellow": "red"): "orange";
				const char *diffname = bbi->diff? (\
				bbi->diff->type==R_ANAL_DIFF_TYPE_MATCH? "match":
				bbi->diff->type==R_ANAL_DIFF_TYPE_UNMATCH? "unmatch": "new"): "unk";
				if (is_keva) {
					sdb_set (DB, "diff", diffname, 0);
					sdb_set (DB, "label", str, 0);
				} else if (!is_json) {
					nodes++;
					RConfigHold *hc = r_config_hold_new (core->config);
					r_config_hold (hc, "scr.color", "scr.utf8", "asm.offset", "asm.lines",
							"asm.cmt.right", "asm.lines.fcn", "asm.bytes", NULL);
					RDiff *d = r_diff_new ();
					r_config_set_i (core->config, "scr.utf8", 0);
					r_config_set_i (core->config, "asm.offset", 0);
					r_config_set_i (core->config, "asm.lines", 0);
					r_config_set_i (core->config, "asm.cmt.right", 0);
					r_config_set_i (core->config, "asm.lines.fcn", 0);
					r_config_set_i (core->config, "asm.bytes", 0);
					if (!is_star) {
						r_config_set_i (core->config, "scr.color", 0);	// disable color for dot
					}

					if (bbi->diff && bbi->diff->type != R_ANAL_DIFF_TYPE_MATCH && core->c2) {
						RCore *c = core->c2;
						RConfig *oc = c->config;
						char *str = r_core_cmd_strf (core, "pdb @ 0x%08"PFMT64x, bbi->addr);
						c->config = core->config;
						// XXX. the bbi->addr doesnt needs to be in the same address in core2
						char *str2 = r_core_cmd_strf (c, "pdb @ 0x%08"PFMT64x, bbi->diff->addr);
						char *diffstr = r_diff_buffers_to_string (d,
								(const ut8*)str, strlen (str),
								(const ut8*)str2, strlen (str2));

						if (diffstr) {
							char *nl = strchr (diffstr, '\n');
							if (nl) {
								nl = strchr (nl + 1, '\n');
								if (nl) {
									nl = strchr (nl + 1, '\n');
									if (nl) {
										r_str_cpy (diffstr, nl + 1);
									}
								}
							}
						}

						if (is_star) {
							char *title = get_title (bbi->addr);
							char *body_b64 = r_base64_encode_dyn (diffstr, -1);
							if (!title  || !body_b64) {
								free (body_b64);
								free (title);
								r_diff_free (d);
								return false;
							}
							body_b64 = r_str_prepend (body_b64, "base64:");
							r_cons_printf ("agn %s %s %d\n", title, body_b64, bbi->diff->type);
							free (body_b64);
							free (title);
						} else {
							diffstr = r_str_replace (diffstr, "\n", "\\l", 1);
							diffstr = r_str_replace (diffstr, "\"", "'", 1);
							r_cons_printf(" \"0x%08"PFMT64x"\" [fillcolor=\"%s\","
							"color=\"black\", fontname=\"%s\","
							" label=\"%s\", URL=\"%s/0x%08"PFMT64x"\"]\n",
							bbi->addr, difftype, font, diffstr, fcn->name,
							bbi->addr);
						}
						free (diffstr);
						c->config = oc;
					} else {
						if (is_star) {
							char *title = get_title (bbi->addr);
							char *body_b64 = r_base64_encode_dyn (str, -1);
							int color = (bbi && bbi->diff) ? bbi->diff->type : 0;
							if (!title  || !body_b64) {
								free (body_b64);
								free (title);
								r_diff_free (d);
								return false;
							}
							body_b64 = r_str_prepend (body_b64, "base64:");
							r_cons_printf ("agn %s %s %d\n", title, body_b64, color);
							free (body_b64);
							free (title);
						} else {
							r_cons_printf(" \"0x%08"PFMT64x"\" [fillcolor=\"%s\","
									"color=\"black\", fontname=\"%s\","
									" label=\"%s\", URL=\"%s/0x%08"PFMT64x"\"]\n",
									bbi->addr, difftype, font, str, fcn->name, bbi->addr);
						}
					}
					r_diff_free (d);
					r_config_set_i (core->config, "scr.color", 1);
					r_config_hold_free (hc);
				}
			} else {
				if (is_html) {
						nodes++;
						r_cons_printf ("<p class=\"block draggable\" style=\""
												"top: %dpx; left: %dpx; width: 400px;\" id=\""
												"_0x%08"PFMT64x"\">\n%s</p>\n",
												top, left, bbi->addr, str);
						left = left? 0: 600;
						if (!left) {
								top += 250;
						}
				} else if (!is_json && !is_keva) {
					bool current = r_anal_block_contains (bbi, core->offset);
					const char *label_color = bbi->traced
							? pal_traced
							: (current && color_current)
							? pal_curr
							: pal_box4;
					const char *fill_color = ((current && color_current) || label_color == pal_traced)? pal_traced: "white";
					nodes++;
					if (is_star) {
						char *title = get_title (bbi->addr);
						char *body_b64 = r_base64_encode_dyn (str, -1);
						int color = (bbi && bbi->diff) ? bbi->diff->type : 0;
						if (!title  || !body_b64) {
								free (body_b64);
								free (title);
								return false;
						}
						body_b64 = r_str_prepend (body_b64, "base64:");
						r_cons_printf ("agn %s %s %d\n", title, body_b64, color);
						free (body_b64);
						free (title);
					} else {
						r_cons_printf ("\t\"0x%08"PFMT64x"\" ["
								"URL=\"%s/0x%08"PFMT64x"\", fillcolor=\"%s\","
								"color=\"%s\", fontname=\"%s\","
								"label=\"%s\"]\n",
								bbi->addr, fcn->name, bbi->addr,
								fill_color, label_color, font, str);
					}
				}
			}
			free (str);
		}
	}
	return nodes;
}