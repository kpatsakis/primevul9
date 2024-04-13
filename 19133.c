R_API void r_core_anal_callgraph(RCore *core, ut64 addr, int fmt) {
	const char *font = r_config_get (core->config, "graph.font");
	int is_html = r_cons_context ()->is_html;
	bool refgraph = r_config_get_i (core->config, "graph.refs");
	RListIter *iter, *iter2;
	int usenames = r_config_get_i (core->config, "graph.json.usenames");;
	RAnalFunction *fcni;
	RAnalRef *fcnr;
	PJ *pj = NULL;

	ut64 from = r_config_get_i (core->config, "graph.from");
	ut64 to = r_config_get_i (core->config, "graph.to");

	switch (fmt) {
	case R_GRAPH_FORMAT_JSON:
		pj = pj_new ();
		if (!pj) {
			return;
		}
		pj_a (pj);
		break;
	case R_GRAPH_FORMAT_GML:
	case R_GRAPH_FORMAT_GMLFCN:
		r_cons_printf ("graph\n[\n"
				"hierarchic  1\n"
				"label  \"\"\n"
				"directed  1\n");
		break;
	case R_GRAPH_FORMAT_DOT:
		if (!is_html) {
			const char * gv_edge = r_config_get (core->config, "graph.gv.edge");
			char * gv_node = strdup (r_config_get (core->config, "graph.gv.node"));
			const char * gv_grph = r_config_get (core->config, "graph.gv.graph");
			const char * gv_spline = r_config_get (core->config, "graph.gv.spline");
			if (!gv_edge || !*gv_edge) {
				gv_edge = "arrowhead=\"normal\" style=bold weight=2";
			}
			if (!gv_node || !*gv_node) {
				const char *font = r_config_get (core->config, "graph.font");
				free (gv_node);
				gv_node = r_str_newf ("penwidth=4 fillcolor=white style=filled fontname=\"%s Bold\" fontsize=14 shape=box", font);
			}
			if (!gv_grph || !*gv_grph) {
				gv_grph = "bgcolor=azure";
			}
			if (!gv_spline || !*gv_spline) {
				// ortho for bbgraph and curved for callgraph
				gv_spline = "splines=\"curved\"";
			}
			r_cons_printf ("digraph code {\n"
					"rankdir=LR;\n"
					"outputorder=edgesfirst;\n"
					"graph [%s fontname=\"%s\" %s];\n"
					"node [%s];\n"
					"edge [%s];\n", gv_grph, font, gv_spline,
					gv_node, gv_edge);
			free (gv_node);
		}
		break;
	}
	ut64 base = UT64_MAX;
	int iteration = 0;
repeat:
	r_list_foreach (core->anal->fcns, iter, fcni) {
		if (base == UT64_MAX) {
			base = fcni->addr;
		}
		if (from != UT64_MAX && fcni->addr < from) {
			continue;
		}
		if (to != UT64_MAX && fcni->addr > to) {
			continue;
		}
		if (addr != UT64_MAX && addr != fcni->addr) {
			continue;
		}
		RList *refs = r_anal_function_get_refs (fcni);
		RList *calls = r_list_new ();
		// TODO: maybe fcni->calls instead ?
		r_list_foreach (refs, iter2, fcnr) {
			//  TODO: tail calll jumps are also calls
			if (fcnr->type == 'C' && r_list_find(calls, fcnr, (RListComparator)RAnalRef_cmp) == NULL) {
				r_list_append (calls, fcnr);
			}
		}
		if (r_list_empty(calls)) {
			r_list_free (refs);
			r_list_free (calls);
			continue;
		}
		switch (fmt) {
		case R_GRAPH_FORMAT_NO:
			r_cons_printf ("0x%08"PFMT64x"\n", fcni->addr);
			break;
		case R_GRAPH_FORMAT_GML:
		case R_GRAPH_FORMAT_GMLFCN: {
			RFlagItem *flag = r_flag_get_i (core->flags, fcni->addr);
			if (iteration == 0) {
				char *msg = flag? strdup (flag->name): r_str_newf ("0x%08"PFMT64x, fcni->addr);
				r_cons_printf ("  node [\n"
						"  id  %"PFMT64d"\n"
						"    label  \"%s\"\n"
						"  ]\n", fcni->addr - base, msg);
				free (msg);
			}
			break;
		}
		case R_GRAPH_FORMAT_JSON:
			pj_o (pj);
			if (usenames) {
				pj_ks (pj, "name", fcni->name);
			} else {
				char fcni_addr[20];
				snprintf (fcni_addr, sizeof (fcni_addr) - 1, "0x%08" PFMT64x, fcni->addr);
				pj_ks (pj, "name", fcni_addr);
			}
			pj_kn (pj, "size", r_anal_function_linear_size (fcni));
			pj_ka (pj, "imports");
			break;
		case R_GRAPH_FORMAT_DOT:
			r_cons_printf ("  \"0x%08"PFMT64x"\" "
					"[label=\"%s\""
					" URL=\"%s/0x%08"PFMT64x"\"];\n",
					fcni->addr, fcni->name,
					fcni->name, fcni->addr);
		}
		r_list_foreach (calls, iter2, fcnr) {
			// TODO: display only code or data refs?
			RFlagItem *flag = r_flag_get_i (core->flags, fcnr->addr);
			char *fcnr_name = (flag && flag->name) ? flag->name : r_str_newf ("unk.0x%"PFMT64x, fcnr->addr);
			switch (fmt) {
			case R_GRAPH_FORMAT_GMLFCN:
				if (iteration == 0) {
					r_cons_printf ("  node [\n"
							"    id  %"PFMT64d"\n"
							"    label  \"%s\"\n"
							"  ]\n", fcnr->addr - base, fcnr_name);
					r_cons_printf ("  edge [\n"
							"    source  %"PFMT64d"\n"
							"    target  %"PFMT64d"\n"
							"  ]\n", fcni->addr-base, fcnr->addr-base);
				}
			case R_GRAPH_FORMAT_GML:
				if (iteration != 0) {
					r_cons_printf ("  edge [\n"
							"    source  %"PFMT64d"\n"
							"    target  %"PFMT64d"\n"
							"  ]\n", fcni->addr-base, fcnr->addr-base); //, "#000000"
				}
				break;
			case R_GRAPH_FORMAT_DOT:
				r_cons_printf ("  \"0x%08"PFMT64x"\" -> \"0x%08"PFMT64x"\" "
						"[color=\"%s\" URL=\"%s/0x%08"PFMT64x"\"];\n",
						//"[label=\"%s\" color=\"%s\" URL=\"%s/0x%08"PFMT64x"\"];\n",
						fcni->addr, fcnr->addr, //, fcnr_name,
						"#61afef",
						fcnr_name, fcnr->addr);
				r_cons_printf ("  \"0x%08"PFMT64x"\" "
						"[label=\"%s\""
						" URL=\"%s/0x%08"PFMT64x"\"];\n",
						fcnr->addr, fcnr_name,
						fcnr_name, fcnr->addr);
				break;
			case R_GRAPH_FORMAT_JSON:
				if (usenames) {
					pj_s (pj, fcnr_name);
				} else {
					char fcnr_addr[20];
					snprintf (fcnr_addr, sizeof (fcnr_addr) - 1, "0x%08" PFMT64x, fcnr->addr);
					pj_s (pj, fcnr_addr);
				}
				break;
			default:
				if (refgraph || fcnr->type == R_ANAL_REF_TYPE_CALL) {
					// TODO: avoid recreating nodes unnecessarily
					r_cons_printf ("agn %s\n", fcni->name);
					r_cons_printf ("agn %s\n", fcnr_name);
					r_cons_printf ("age %s %s\n", fcni->name, fcnr_name);
				} else {
					r_cons_printf ("# - 0x%08"PFMT64x" (%c)\n", fcnr->addr, fcnr->type);
				}
			}
			if (!(flag && flag->name)) {
				free(fcnr_name);
			}
		}
		r_list_free (refs);
		r_list_free (calls);
		if (fmt == R_GRAPH_FORMAT_JSON) {
			pj_end (pj);
			pj_end (pj);
		}
	}
	if (iteration == 0 && fmt == R_GRAPH_FORMAT_GML) {
		iteration++;
		goto repeat;
	}
	if (iteration == 0 && fmt == R_GRAPH_FORMAT_GMLFCN) {
		iteration++;
	}
	switch (fmt) {
	case R_GRAPH_FORMAT_GML:
	case R_GRAPH_FORMAT_GMLFCN:
	case R_GRAPH_FORMAT_JSON:
		pj_end (pj);
		r_cons_println (pj_string (pj));
		pj_free (pj);
		break;
	case R_GRAPH_FORMAT_DOT:
		r_cons_printf ("}\n");
		break;
	}
}