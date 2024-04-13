static void cmd_anal_graph(RCore *core, const char *input) {
	core->graph->show_node_titles = r_config_get_i (core->config, "graph.ntitles");
	r_cons_enable_highlight (false);
	switch (input[0]) {
	case 'f': // "agf"
		switch (input[1]) {
		case 0: // "agf"
			r_core_visual_graph (core, NULL, NULL, false);
			break;
		case ' ':{ // "agf "
			RAnalFunction *fcn = r_anal_get_fcn_in (core->anal, core->offset, 0);
			r_core_visual_graph (core, NULL, fcn, false);
			break;
			}
		case 'v': // "agfv"
			eprintf ("\rRendering graph...");
			RAnalFunction *fcn = r_anal_get_fcn_in (core->anal, core->offset, R_ANAL_FCN_TYPE_ROOT);
			if (fcn) {
				r_core_visual_graph (core, NULL, fcn, 1);
			}
			r_cons_enable_mouse (false);
			r_cons_show_cursor (true);
			break;
		case 't': { // "agft" - tiny graph
			int e = r_config_get_i (core->config, "graph.edges");
			r_config_set_i (core->config, "graph.edges", 0);
			RAnalFunction *fcn = r_anal_get_fcn_in (core->anal, core->offset, 0);
			r_core_visual_graph (core, NULL, fcn, 2);
			r_config_set_i (core->config, "graph.edges", e);
			break;
			}
		case 'd': // "agfd"
			if (input[2] == 'm') {
				r_core_anal_graph (core, r_num_math (core->num, input + 3),
					R_CORE_ANAL_GRAPHLINES);
			} else {
				r_core_anal_graph (core, r_num_math (core->num, input + 2),
					R_CORE_ANAL_GRAPHBODY);
			}
			break;
		case 'j': // "agfj"
			r_core_anal_graph (core, r_num_math (core->num, input + 2), R_CORE_ANAL_JSON);
			break;
		case 'J': {// "agfJ"
			// Honor asm.graph=false in json as well
			RConfigHold *hc = r_config_hold_new (core->config);
			r_config_hold_i (hc, "asm.offset", NULL);
			const bool o_graph_offset = r_config_get_i (core->config, "graph.offset");
			r_config_set_i (core->config, "asm.offset", o_graph_offset);
			r_core_anal_graph (core, r_num_math (core->num, input + 2),
				R_CORE_ANAL_JSON | R_CORE_ANAL_JSON_FORMAT_DISASM);
			r_config_hold_restore (hc);
			r_config_hold_free (hc);
			break;
			}
		case 'g':{ // "agfg"
			RAnalFunction *fcn = r_anal_get_fcn_in (core->anal, core->offset, 0);
			r_core_print_bb_gml (core, fcn);
			break;
			}
		case 'k':{ // "agfk"
			r_core_cmdf (core, "ag-; .agf* @ %"PFMT64u"; aggk", core->offset);
			break;
			}
		case '*':{// "agf*"
			RAnalFunction *fcn = r_anal_get_fcn_in (core->anal, core->offset, 0);
			r_core_print_bb_custom (core, fcn);
			break;
			}
		case 'w': // "agfw"
			if (r_config_get_i (core->config, "graph.web")) {
				r_core_cmd0 (core, "=H /graph/");
			} else {
				char *cmdargs = r_str_newf ("agfd @ 0x%"PFMT64x, core->offset);
				char *cmd = graph_cmd (core, cmdargs, input + 2);
				if (cmd && *cmd) {
					if (*(input + 2)) {
						r_cons_printf ("Saving to file %s ...\n", input + 2);
						r_cons_flush ();
					}
					r_core_cmd0 (core, cmd);
				}
				free (cmd);
				free (cmdargs);
			}
			break;
		default:
			eprintf ("Usage: see ag?\n");
			break;
		}
		break;
	case '-': // "ag-"
		r_agraph_reset (core->graph);
		break;
	case 'n': // "agn"
		cmd_agraph_node (core, input + 1);
		break;
	case 'e': // "age"
		cmd_agraph_edge (core, input + 1);
		break;
	case 'g': // "agg"
		r_core_agraph_print (core, -1, input + 1);
		break;
	case 's': // "ags"
		r_core_anal_graph (core, r_num_math (core->num, input + 1), 0);
		break;
	case 'C': // "agC"
		switch (input[1]) {
		case 'v':
		case 't':
		case 'k':
		case 'w':
		case ' ':
		case 0: {
			core->graph->is_callgraph = true;
			r_core_cmdf (core, "ag-; .agC*;");
			r_core_agraph_print(core, -1, input + 1);
			core->graph->is_callgraph = false;
			break;
			}
		case 'J':
		case 'j':
			r_core_anal_callgraph (core, UT64_MAX, R_GRAPH_FORMAT_JSON);
			break;
		case 'g':
			r_core_anal_callgraph (core, UT64_MAX, R_GRAPH_FORMAT_GML);
			break;
		case 'd':
			r_core_anal_callgraph (core, UT64_MAX, R_GRAPH_FORMAT_DOT);
			break;
		case '*':
			r_core_anal_callgraph (core, UT64_MAX, R_GRAPH_FORMAT_CMD);
			break;
		default:
			eprintf ("Usage: see ag?\n");
			break;
		}
		break;
	case 'r': // "agr" references graph
		switch (input[1]) {
		case '*': {
			r_core_anal_coderefs (core, core->offset);
			}
			break;
		default: {
			core->graph->is_callgraph = true;
			r_core_cmdf (core, "ag-; .agr* @ %"PFMT64u";", core->offset);
			r_core_agraph_print(core, -1, input + 1);
			core->graph->is_callgraph = false;
			break;
			}
		}
		break;
	case 'R': // "agR" global refs
		switch (input[1]) {
		case '*': {
			ut64 from = r_config_get_i (core->config, "graph.from");
			ut64 to = r_config_get_i (core->config, "graph.to");
			RListIter *it;
			RAnalFunction *fcn;
			r_list_foreach (core->anal->fcns, it, fcn) {
				if ((from == UT64_MAX && to == UT64_MAX) || R_BETWEEN (from, fcn->addr, to)) {
					r_core_anal_coderefs (core, fcn->addr);
				}
			}
			break;
			}
		default: {
			core->graph->is_callgraph = true;
			r_core_cmdf (core, "ag-; .agR*;");
			r_core_agraph_print(core, -1, input + 1);
			core->graph->is_callgraph = false;
			break;
			}
		}
		break;
	case 'x': // "agx" cross refs
		switch (input[1]) {
		case '*': {
			r_core_anal_codexrefs (core, core->offset);
			}
			break;
		default: {
			r_core_cmdf (core, "ag-; .agx* @ %"PFMT64u";", core->offset);
			r_core_agraph_print(core, -1, input + 1);
			break;
			}
		}
		break;
	case 'i': // "agi" import graph
		switch (input[1]) {
		case '*':
			r_core_anal_importxrefs (core);
			break;
		default:
			r_core_cmdf (core, "ag-; .agi*;");
			r_core_agraph_print(core, -1, input + 1);
			break;
		}
		break;
	case 'c': // "agc"
		switch (input[1]) {
		case 'v':
		case 't':
		case 'k':
		case 'w':
		case ' ': {
			core->graph->is_callgraph = true;
			r_core_cmdf (core, "ag-; .agc* @ %"PFMT64u"; agg%s;", core->offset, input + 1);
			core->graph->is_callgraph = false;
			break;
			}
		case 0:
			core->graph->is_callgraph = true;
			r_core_cmd0 (core, "ag-; .agc* $$; agg;");
			core->graph->is_callgraph = false;
			break;
		case 'g': {
			r_core_anal_callgraph (core, core->offset, R_GRAPH_FORMAT_GMLFCN);
			break;
		}
		case 'd': {
			r_core_anal_callgraph (core, core->offset, R_GRAPH_FORMAT_DOT);
			break;
		}
		case 'J':
		case 'j': {
			r_core_anal_callgraph (core, core->offset, R_GRAPH_FORMAT_JSON);
			break;
		}
		case '*': {
			r_core_anal_callgraph (core, core->offset, R_GRAPH_FORMAT_CMD);
			break;
		}
		default:
			eprintf ("Usage: see ag?\n");
			break;
		}
		break;
	case 'j': // "agj" alias for agfj
		r_core_cmdf (core, "agfj%s", input + 1);
		break;
	case 'J': // "agJ" alias for agfJ
		r_core_cmdf (core, "agfJ%s", input + 1);
		break;
	case 'k': // "agk" alias for agfk
		r_core_cmdf (core, "agfk%s", input + 1);
		break;
	case 'l': // "agl"
		r_core_anal_graph (core, r_num_math (core->num, input + 1), R_CORE_ANAL_GRAPHLINES);
		break;
	case 'a': // "aga"
		switch (input[1]) {
		case '*': {
			r_core_anal_datarefs (core, core->offset);
			break;
			}
		default:
			r_core_cmdf (core, "ag-; .aga* @ %"PFMT64u";", core->offset);
			r_core_agraph_print(core, -1, input + 1);
			break;
		}
		break;
	case 'A': // "agA" global data refs
		switch (input[1]) {
		case '*': {
			ut64 from = r_config_get_i (core->config, "graph.from");
			ut64 to = r_config_get_i (core->config, "graph.to");
			RListIter *it;
			RAnalFunction *fcn;
			r_list_foreach (core->anal->fcns, it, fcn) {
				if ((from == UT64_MAX && to == UT64_MAX) || R_BETWEEN (from, fcn->addr, to)) {
					r_core_anal_datarefs (core, fcn->addr);
				}
			}
			break;
			}
		default:
			r_core_cmdf (core, "ag-; .agA*;");
			r_core_agraph_print(core, -1, input + 1);
			break;
		}
		break;
	case 'd': {// "agd"
	        int diff_opt = R_CORE_ANAL_GRAPHBODY | R_CORE_ANAL_GRAPHDIFF;
                switch (input[1]) {
                        case 'j': {
                                ut64 addr = input[2] ? r_num_math (core->num, input + 2) : core->offset;
                                r_core_gdiff_fcn (core, addr, core->offset);
                                r_core_anal_graph (core, addr, diff_opt | R_CORE_ANAL_JSON);
                                break;
                        }
                        case 'J': {
                                ut64 addr = input[2] ? r_num_math (core->num, input + 2) : core->offset;
                                r_core_gdiff_fcn (core, addr, core->offset);
                                r_core_anal_graph (core, addr, diff_opt | R_CORE_ANAL_JSON | R_CORE_ANAL_JSON_FORMAT_DISASM);
                                break;
                        }
                        case '*': {
                                ut64 addr = input[2] ? r_num_math (core->num, input + 2) : core->offset;
                                r_core_gdiff_fcn (core, addr, core->offset);
                                r_core_anal_graph (core, addr, diff_opt | R_CORE_ANAL_STAR);
                                break;
                        }
                        case ' ':
                        case 0:
                        case 't':
                        case 'k':
                        case 'v':
                        case 'g': {
                                ut64 addr = input[2] ? r_num_math (core->num, input + 2) : core->offset;
                                r_core_cmdf (core, "ag-; .agd* @ %"PFMT64u"; agg%s;", addr, input + 1);
                                break;
                        }
                        case 'd': {
                                ut64 addr = input[2] ? r_num_math (core->num, input + 2) : core->offset;
                                r_core_gdiff_fcn (core, addr, core->offset);
                                r_core_anal_graph (core, addr, diff_opt);
                                break;
                        }
                        case 'w': {
                                char *cmdargs = r_str_newf ("agdd 0x%"PFMT64x, core->offset);
                                char *cmd = graph_cmd (core, cmdargs, input + 2);
                                if (cmd && *cmd) {
                                        r_core_cmd0(core, cmd);
                                }
                                free(cmd);
                                free(cmdargs);
                                break;
                        }
                }
                break;
        }
	case 'v': // "agv" alias for "agfv"
		r_core_cmdf (core, "agfv%s", input + 1);
		break;
	case 'w':// "agw"
		if (r_config_get_i (core->config, "graph.web")) {
			r_core_cmd0 (core, "=H /graph/");
		} else {
			char *cmdargs = r_str_newf ("agfd @ 0x%"PFMT64x, core->offset);
			char *cmd = graph_cmd (core, cmdargs, input + 1);
			if (cmd && *cmd) {
				if (input[1]) {
					r_cons_printf ("Saving to file %s ...\n", input + 1);
					r_cons_flush ();
				}
				r_core_cmd0 (core, cmd);
			}
			free (cmd);
			free (cmdargs);
		}
		break;
	default:
		r_core_cmd_help (core, help_msg_ag);
		break;
	}
}