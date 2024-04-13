static bool cmd_anal_aaft(RCore *core) {
	RListIter *it;
	RAnalFunction *fcn;
	ut64 seek;
	const char *io_cache_key = "io.pcache.write";
	bool io_cache = r_config_get_i (core->config, io_cache_key);
	if (r_config_get_i (core->config, "cfg.debug")) {
		eprintf ("TOFIX: aaft can't run in debugger mode.\n");
		return false;
	}
	if (!io_cache) {
		// XXX. we shouldnt need this, but it breaks 'r2 -c aaa -w ls'
		r_config_set_i (core->config, io_cache_key, true);
	}
	seek = core->offset;
	r_reg_arena_push (core->anal->reg);
	// Iterating Reverse so that we get function in top-bottom call order
	r_list_foreach_prev (core->anal->fcns, it, fcn) {
		r_core_cmd0 (core, "aei");
		r_core_cmd0 (core, "aeim");
		int ret = r_core_seek (core, fcn->addr, true);
		if (!ret) {
			continue;
		}
		r_anal_esil_set_pc (core->anal->esil, fcn->addr);
		r_core_anal_type_match (core, fcn);
		r_core_cmd0 (core, "aeim-");
		r_core_cmd0 (core, "aei-");
		if (r_cons_is_breaked ()) {
			break;
		}
		__add_vars_sdb (core, fcn);
	}
	r_core_seek (core, seek, true);
	r_reg_arena_pop (core->anal->reg);
	r_config_set_i (core->config, io_cache_key, io_cache);
	return true;
}