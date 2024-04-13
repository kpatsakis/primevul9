static void cmd_agraph_edge(RCore *core, const char *input) {
	switch (*input) {
	case ' ': // "age"
	case '-': { // "age-"
		RANode *u, *v;
		char **args;
		int n_args;

		args = r_str_argv (input + 1, &n_args);
		if (n_args != 2) {
			r_cons_printf ("Wrong arguments\n");
			r_str_argv_free (args);
			break;
		}

		u = r_agraph_get_node (core->graph, args[0]);
		v = r_agraph_get_node (core->graph, args[1]);
		if (!u || !v) {
			if (!u) {
				r_cons_printf ("Node %s not found!\n", args[0]);
			} else {
				r_cons_printf ("Node %s not found!\n", args[1]);
			}
			r_str_argv_free (args);
			break;
		}
		if (*input == ' ') {
			r_agraph_add_edge (core->graph, u, v);
		} else {
			r_agraph_del_edge (core->graph, u, v);
		}
		r_str_argv_free (args);
		break;
	}
	case '?':
	default:
		r_core_cmd_help (core, help_msg_age);
		break;
	}
}