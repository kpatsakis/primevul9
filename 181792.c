static void __add_vars_sdb(RCore *core, RAnalFunction *fcn) {
	RAnalFcnVarsCache cache;
	r_anal_fcn_vars_cache_init (core->anal, &cache, fcn);
	RListIter *iter;
	RAnalVar *var;
	int arg_count = 0;

	RList *all_vars = cache.rvars;
	r_list_join (all_vars, cache.bvars);
	r_list_join (all_vars, cache.svars);

	r_list_foreach (all_vars, iter, var) {
		if (var->isarg) {
			char *query = r_str_newf ("anal/types/func.%s.arg.%d=%s,%s", fcn->name, arg_count, var->type, var->name);
			sdb_querys (core->sdb, NULL, 0, query);
			free (query);
			arg_count++;
		}
	}
	if (arg_count > 0) {
		char *query = r_str_newf ("anal/types/func.%s.args=%d", fcn->name, arg_count);
		sdb_querys (core->sdb, NULL, 0, query);
		free (query);
	}
	r_anal_fcn_vars_cache_fini (&cache);
}