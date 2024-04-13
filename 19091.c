R_API int r_core_anal_graph(RCore *core, ut64 addr, int opts) {
	ut64 from = r_config_get_i (core->config, "graph.from");
	ut64 to = r_config_get_i (core->config, "graph.to");
	const char *font = r_config_get (core->config, "graph.font");
	int is_html = r_cons_context ()->is_html;
	int is_json = opts & R_CORE_ANAL_JSON;
	int is_json_format_disasm = opts & R_CORE_ANAL_JSON_FORMAT_DISASM;
	int is_keva = opts & R_CORE_ANAL_KEYVALUE;
	int is_star = opts & R_CORE_ANAL_STAR;
	RConfigHold *hc;
	RAnalFunction *fcni;
	RListIter *iter;
	int nodes = 0;
	PJ *pj = NULL;

	if (!addr) {
		addr = core->offset;
	}
	if (r_list_empty (core->anal->fcns)) {
		return false;
	}
	hc = r_config_hold_new (core->config);
	if (!hc) {
		return false;
	}

	r_config_hold (hc, "asm.lines", "asm.bytes", "asm.dwarf", NULL);
	//opts |= R_CORE_ANAL_GRAPHBODY;
	r_config_set_i (core->config, "asm.lines", 0);
	r_config_set_i (core->config, "asm.dwarf", 0);
	if (!is_json_format_disasm) {
		r_config_hold (hc, "asm.bytes", NULL);
		r_config_set_i (core->config, "asm.bytes", 0);
	}
	if (!is_html && !is_json && !is_keva && !is_star) {
		const char * gv_edge = r_config_get (core->config, "graph.gv.edge");
		const char * gv_node = r_config_get (core->config, "graph.gv.node");
		const char * gv_spline = r_config_get (core->config, "graph.gv.spline");
		if (!gv_edge || !*gv_edge) {
			gv_edge = "arrowhead=\"normal\"";
		}
		if (!gv_node || !*gv_node) {
			gv_node = "fillcolor=gray style=filled shape=box";
		}
		if (!gv_spline || !*gv_spline) {
			gv_spline = "splines=\"ortho\"";
		}
		r_cons_printf ("digraph code {\n"
			"\tgraph [bgcolor=azure fontsize=8 fontname=\"%s\" %s];\n"
			"\tnode [%s];\n"
			"\tedge [%s];\n", font, gv_spline, gv_node, gv_edge);
	}
	if (is_json) {
		pj = r_core_pj_new (core);
		if (!pj) {
			r_config_hold_restore (hc);
			r_config_hold_free (hc);
			return false;
		}
		pj_a (pj);
	}
	r_list_foreach (core->anal->fcns, iter, fcni) {
		if (fcni->type & (R_ANAL_FCN_TYPE_SYM | R_ANAL_FCN_TYPE_FCN |
						  R_ANAL_FCN_TYPE_LOC) &&
			(addr == UT64_MAX || r_anal_get_fcn_in (core->anal, addr, 0) == fcni)) {
			if (addr == UT64_MAX && (from != UT64_MAX && to != UT64_MAX)) {
				if (fcni->addr < from || fcni->addr > to) {
					continue;
				}
			}
			nodes += core_anal_graph_nodes (core, fcni, opts, pj);
			if (addr != UT64_MAX) {
				break;
			}
		}
	}
	if (!nodes) {
		if (!is_html && !is_json && !is_keva) {
			RAnalFunction *fcn = r_anal_get_fcn_in (core->anal, addr, 0);
			if (is_star) {
					char *name = get_title(fcn ? fcn->addr: addr);
					r_cons_printf ("agn %s;", name);
			} else {
				r_cons_printf ("\t\"0x%08"PFMT64x"\";\n", fcn? fcn->addr: addr);
			}
		}
	}
	if (!is_keva && !is_html && !is_json && !is_star && !is_json_format_disasm) {
		r_cons_printf ("}\n");
	}
	if (is_json) {
		pj_end (pj);
		r_cons_printf ("%s\n", pj_string (pj));
		pj_free (pj);
	}
	r_config_hold_restore (hc);
	r_config_hold_free (hc);
	return true;
}