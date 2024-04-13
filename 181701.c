static void cmd_anal_fcn_sig(RCore *core, const char *input) {
	bool json = (input[0] == 'j');
	char *p = strchr (input, ' ');
	char *fcn_name = p ? r_str_trim_dup (p): NULL;
	RListIter *iter;
	RAnalFuncArg *arg;

	RAnalFunction *fcn;
	if (fcn_name) {
		fcn = r_anal_fcn_find_name (core->anal, fcn_name);
	} else {
		fcn = r_anal_get_fcn_in (core->anal, core->offset, 0);
		if (fcn) {
			fcn_name = fcn->name;
		}
	}
	if (!fcn) {
		return;
	}

	if (json) {
		PJ *j = pj_new ();
		if (!j) {
			return;
		}
		pj_a (j);

		char *key = NULL;
		if (fcn_name) {
			key = resolve_fcn_name (core->anal, fcn_name);
		}

		if (key) {
			const char *fcn_type = r_type_func_ret (core->anal->sdb_types, key);
			int nargs = r_type_func_args_count (core->anal->sdb_types, key);
			if (fcn_type) {
				pj_o (j);
				pj_ks (j, "name", r_str_get (key));
				pj_ks (j, "return", r_str_get (fcn_type));
				pj_k (j, "args");
				pj_a (j);
				if (nargs) {
					RList *list = r_core_get_func_args (core, fcn_name);
					r_list_foreach (list, iter, arg) {
						char *type = arg->orig_c_type;
						pj_o (j);
						pj_ks (j, "name", arg->name);
						pj_ks (j, "type", type);
						pj_end (j);
					}
					r_list_free (list);
				}
				pj_end (j);
				pj_ki (j, "count", nargs);
				pj_end (j);
			}
			free (key);
		} else {
			pj_o (j);
			pj_ks (j, "name", r_str_get (fcn_name));
			pj_k (j, "args");
			pj_a (j);

			RAnalFcnVarsCache cache;
			r_anal_fcn_vars_cache_init (core->anal, &cache, fcn);
			int nargs = 0;
			RAnalVar *var;
			r_list_foreach (cache.rvars, iter, var) {
				nargs++;
				pj_o (j);
				pj_ks (j, "name", var->name);
				pj_ks (j, "type", var->type);
				pj_end (j);
			}
			r_list_foreach (cache.bvars, iter, var) {
				if (var->delta <= 0) {
					continue;
				}
				nargs++;
				pj_o (j);
				pj_ks (j, "name", var->name);
				pj_ks (j, "type", var->type);
				pj_end (j);
			}
			r_list_foreach (cache.svars, iter, var) {
				if (!var->isarg) {
					continue;
				}
				nargs++;
				pj_o (j);
				pj_ks (j, "name", var->name);
				pj_ks (j, "type", var->type);
				pj_end (j);
			}
			r_anal_fcn_vars_cache_fini (&cache);

			pj_end (j);
			pj_ki (j, "count", nargs);
			pj_end (j);
		}
		pj_end (j);
		const char *s = pj_string (j);
		if (s) {
			r_cons_printf ("%s\n", s);
		}
		pj_free (j);
	} else {
		char *sig = r_anal_fcn_format_sig (core->anal, fcn, fcn_name, NULL, NULL, NULL);
		if (sig) {
			r_cons_printf ("%s\n", sig);
			free (sig);
		}
	}
}