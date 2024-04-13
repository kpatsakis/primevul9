static int core_anal_graph_nodes(RCore *core, RAnalFunction *fcn, int opts, PJ *pj) {
	int is_json = opts & R_CORE_ANAL_JSON;
	int is_keva = opts & R_CORE_ANAL_KEYVALUE;
	int nodes = 0;
	Sdb *DB = NULL;
	char *pal_jump = palColorFor ("graph.true");
	char *pal_fail = palColorFor ("graph.false");
	char *pal_trfa = palColorFor ("graph.trufae");
	char *pal_curr = palColorFor ("graph.current");
	char *pal_traced = palColorFor ("graph.traced");
	char *pal_box4 = palColorFor ("graph.box4");
	if (!fcn || !fcn->bbs) {
		eprintf ("No fcn\n");
		free (pal_jump);
		free (pal_fail);
		free (pal_trfa);
		free (pal_curr);
		free (pal_traced);
		free (pal_box4);
		return -1;
	}

	if (is_keva) {
		char ns[64];
		DB = sdb_ns (core->anal->sdb, "graph", 1);
		snprintf (ns, sizeof (ns), "fcn.0x%08"PFMT64x, fcn->addr);
		DB = sdb_ns (DB, ns, 1);
	}

	if (is_keva) {
		char *ename = sdb_encode ((const ut8*)fcn->name, -1);
		sdb_set (DB, "name", fcn->name, 0);
		sdb_set (DB, "ename", ename, 0);
		free (ename);
		sdb_num_set (DB, "size", r_anal_function_linear_size (fcn), 0);
		if (fcn->maxstack > 0) {
			sdb_num_set (DB, "stack", fcn->maxstack, 0);
		}
		sdb_set (DB, "pos", "0,0", 0); // needs to run layout
		sdb_set (DB, "type", r_anal_functiontype_tostring (fcn->type), 0);
	} else if (is_json) {
		// TODO: show vars, refs and xrefs
		char *fcn_name_escaped = r_str_escape_utf8_for_json (fcn->name, -1);
		pj_o (pj);
		pj_ks (pj, "name", r_str_getf (fcn_name_escaped));
		free (fcn_name_escaped);
		pj_kn (pj, "offset", fcn->addr);
		pj_ki (pj, "ninstr", fcn->ninstr);
		pj_ki (pj, "nargs", r_anal_var_count_args (fcn));
		pj_ki (pj, "nlocals", r_anal_var_count_locals (fcn));
		pj_kn (pj, "size", r_anal_function_linear_size (fcn));
		pj_ki (pj, "stack", fcn->maxstack);
		pj_ks (pj, "type", r_anal_functiontype_tostring (fcn->type));
		pj_k (pj, "blocks");
		pj_a (pj);
	}
	nodes += core_anal_graph_construct_nodes (core, fcn, opts, pj, DB);
	nodes += core_anal_graph_construct_edges (core, fcn, opts, pj, DB);
	if (is_json) {
		pj_end (pj);
		pj_end (pj);
	}
	free (pal_jump);
	free (pal_fail);
	free (pal_trfa);
	free (pal_curr);
	free (pal_traced);
	free (pal_box4);
	return nodes;
}