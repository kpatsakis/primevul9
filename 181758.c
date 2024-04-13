static void cmd_anal_info(RCore *core, const char *input) {
	switch (input[0]) {
	case '?':
		r_core_cmd_help (core, help_msg_ai);
		break;
	case ' ':
		cmd_address_info (core, input, 0);
		break;
	case 'i': // "aii"
		// global imports
		if (input[1]) {
			if (input[1] == ' ') {
				if (!core->anal->imports) {
					core->anal->imports = r_list_newf ((RListFree)free);
				}
				r_list_append (core->anal->imports, r_str_trim_dup (input + 1));
			} else if (input[1] == '-') {
				r_list_free (core->anal->imports);
				core->anal->imports = NULL;
			} else {
				eprintf ("Usagae: aii [namespace] # see afii - imports\n");
			}
		} else {
			if (core->anal->imports) {
				char *imp;
				RListIter *iter;
				r_list_foreach (core->anal->imports, iter, imp) {
					r_cons_printf ("%s\n", imp);
				}
			}
		}
		break;
	case 'j': // "aij"
		cmd_address_info (core, input + 1, 'j');
		break;
	default:
		cmd_address_info (core, NULL, 0);
		break;
	}
}