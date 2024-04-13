R_API void r_core_agraph_print (RCore *core, int use_utf, const char *input) {
	if (use_utf != -1) {
		r_config_set_i (core->config, "scr.utf8", use_utf);
	}
	switch (*input) {
	case 0:
		core->graph->can->linemode = r_config_get_i (core->config, "graph.linemode");
		core->graph->can->color = r_config_get_i (core->config, "scr.color");
		r_agraph_set_title (core->graph,
			r_config_get (core->config, "graph.title"));
		r_agraph_print (core->graph);
		break;
	case 't':{ // "aggt" - tiny graph
		core->graph->is_tiny = true;
		int e = r_config_get_i (core->config, "graph.edges");
		r_config_set_i (core->config, "graph.edges", 0);
		r_core_visual_graph (core, core->graph, NULL, false);
		r_config_set_i (core->config, "graph.edges", e);
		core->graph->is_tiny = false;
		break;
		}
	case 'k': // "aggk"
	{
		Sdb *db = r_agraph_get_sdb (core->graph);
		char *o = sdb_querys (db, "null", 0, "*");
		r_cons_print (o);
		free (o);
		break;
	}
	case 'v': // "aggv"
	case 'i': // "aggi" - open current core->graph in interactive mode
	{
		RANode *ran = r_agraph_get_first_node (core->graph);
		if (ran) {
			ut64 oseek = core->offset;
			r_agraph_set_title (core->graph, r_config_get (core->config, "graph.title"));
			r_agraph_set_curnode (core->graph, ran);
			core->graph->force_update_seek = true;
			core->graph->need_set_layout = true;
			core->graph->layout = r_config_get_i (core->config, "graph.layout");
			int ov = r_cons_is_interactive ();
			core->graph->need_update_dim = true;
			int update_seek = r_core_visual_graph (core, core->graph, NULL, true);
			r_config_set_i (core->config, "scr.interactive", ov);
			r_cons_show_cursor (true);
			r_cons_enable_mouse (false);
			if (update_seek != -1) {
				r_core_seek (core, oseek, 0);
			}
		} else {
			eprintf ("This graph contains no nodes\n");
		}
		break;
	}
	case 'd': { // "aggd" - dot format
		const char *font = r_config_get (core->config, "graph.font");
		r_cons_printf ("digraph code {\nrankdir=LR;\noutputorder=edgesfirst\ngraph [bgcolor=azure];\n"
			"edge [arrowhead=normal, color=\"#3030c0\" style=bold weight=2];\n"
			"node [fillcolor=white, style=filled shape=box "
			"fontname=\"%s\" fontsize=\"8\"];\n", font);
		r_agraph_foreach (core->graph, agraph_print_node_dot, NULL);
		r_agraph_foreach_edge (core->graph, agraph_print_edge_dot, NULL);
		r_cons_printf ("}\n");
		break;
		}
	case '*': // "agg*" -
		r_agraph_foreach (core->graph, agraph_print_node, NULL);
		r_agraph_foreach_edge (core->graph, agraph_print_edge, NULL);
		break;
	case 'J':
	case 'j': {
		PJ *pj = pj_new ();
		if (!pj) {
			return;
		}
		pj_o (pj);
		pj_k (pj, "nodes");
		pj_a (pj);
		r_agraph_print_json (core->graph, pj);
		pj_end (pj);
		pj_end (pj);
		r_cons_println (pj_string (pj));
		pj_free (pj);
	} break;
	case 'g':
		r_cons_printf ("graph\n[\n" "hierarchic 1\n" "label \"\"\n" "directed 1\n");
		r_agraph_foreach (core->graph, agraph_print_node_gml, NULL);
		r_agraph_foreach_edge (core->graph, agraph_print_edge_gml, NULL);
		r_cons_print ("]\n");
		break;
	case 'w':{ // "aggw"
		if (r_config_get_i (core->config, "graph.web")) {
			r_core_cmd0 (core, "=H /graph/");
		} else {
			const char *filename = r_str_trim_head_ro (input + 1);
			char *cmd = graph_cmd (core, "aggd", filename);
			if (cmd && *cmd) {
				if (input[1] == ' ') {
					r_cons_printf ("Saving to file '%s'...\n", filename);
					r_cons_flush ();
				}
				r_core_cmd0 (core, cmd);
			}
			free (cmd);
		}
		break;
	}
	default:
		eprintf ("Usage: see ag?\n");
	}
}