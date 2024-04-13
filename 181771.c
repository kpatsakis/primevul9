static void cmd_agraph_node(RCore *core, const char *input) {
	switch (*input) {
	case ' ': { // "agn"
		char *newbody = NULL;
		char **args, *body;
		int n_args, B_LEN = strlen ("base64:");
		int color = -1;
		input++;
		args = r_str_argv (input, &n_args);
		if (n_args < 1 || n_args > 3) {
			r_cons_printf ("Wrong arguments\n");
			r_str_argv_free (args);
			break;
		}
		// strdup cause there is double free in r_str_argv_free due to a realloc call
		if (n_args > 1) {
			body = strdup (args[1]);
			if (strncmp (body, "base64:", B_LEN) == 0) {
				body = r_str_replace (body, "\\n", "", true);
				newbody = (char *)r_base64_decode_dyn (body + B_LEN, -1);
				free (body);
				if (!newbody) {
					eprintf ("Cannot allocate buffer\n");
					r_str_argv_free (args);
					break;
				}
				body = newbody;
			}
			body = r_str_append (body, "\n");
			if (n_args > 2) {
			        color = atoi(args[2]);
			}
		} else {
			body = strdup ("");
		}
		r_agraph_add_node_with_color (core->graph, args[0], body, color);
		r_str_argv_free (args);
		free (body);
		//free newbody it's not necessary since r_str_append reallocate the space
		break;
	}
	case '-': { // "agn-"
		char **args;
		int n_args;

		input++;
		args = r_str_argv (input, &n_args);
		if (n_args != 1) {
			r_cons_printf ("Wrong arguments\n");
			r_str_argv_free (args);
			break;
		}
		r_agraph_del_node (core->graph, args[0]);
		r_str_argv_free (args);
		break;
	}
	case '?':
	default:
		r_core_cmd_help (core, help_msg_agn);
		break;
	}
}