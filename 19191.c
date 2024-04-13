R_API void r_core_anal_paths(RCore *core, ut64 from, ut64 to, bool followCalls, int followDepth, bool is_json) {
	RAnalBlock *b0 = r_anal_bb_from_offset (core->anal, from);
	RAnalBlock *b1 = r_anal_bb_from_offset (core->anal, to);
	PJ *pj = NULL;
	if (!b0) {
		eprintf ("Cannot find basic block for 0x%08"PFMT64x"\n", from);
		return;
	}
	if (!b1) {
		eprintf ("Cannot find basic block for 0x%08"PFMT64x"\n", to);
		return;
	}
	RCoreAnalPaths rcap = {{0}};
	dict_init (&rcap.visited, 32, free);
	rcap.path = r_list_new ();
	rcap.core = core;
	rcap.from = from;
	rcap.fromBB = b0;
	rcap.to = to;
	rcap.toBB = b1;
	rcap.cur = b0;
	rcap.count = r_config_get_i (core->config, "search.maxhits");;
	rcap.followCalls = followCalls;
	rcap.followDepth = followDepth;

	// Initialize a PJ object for json mode
	if (is_json) {
		pj = r_core_pj_new (core);
		pj_a (pj);
	}

	analPaths (&rcap, pj);

	if (is_json) {
		pj_end (pj);
		r_cons_printf ("%s", pj_string (pj));
	}

	if (pj) {
		pj_free (pj);
	}

	dict_fini (&rcap.visited);
	r_list_free (rcap.path);
}