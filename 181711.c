static void cmd_anal_class_base(RCore *core, const char *input) {
	RAnalClassErr err = R_ANAL_CLASS_ERR_SUCCESS;
	char c = input[0];
	switch (c) {
	case ' ': // "acb"
	case '-': { // "acb-"
		const char *str = r_str_trim_head_ro (input + 1);
		if (!*str) {
			eprintf ("No class name given.\n");
			return;
		}
		char *cstr = strdup (str);
		if (!cstr) {
			break;
		}
		char *end = strchr (cstr, ' ');
		if (end) {
			*end = '\0';
			end++;
		}

		if (!end || *end == '\0') {
			if (c == ' ') {
				r_anal_class_list_bases (core->anal, cstr);
			} else /*if (c == '-')*/ {
				eprintf ("No base id given.\n");
			}
			free (cstr);
			break;
		}

		char *base_str = end;
		end = strchr (base_str, ' ');
		if (end) {
			*end = '\0';
		}

		if (c == '-') {
			err = r_anal_class_base_delete (core->anal, cstr, base_str);
			free (cstr);
			break;
		}

		RAnalBaseClass base;
		base.id = NULL;
		base.offset = 0;
		base.class_name = base_str;

		if (end) {
			base.offset = r_num_get (core->num, end + 1);
		}

		err = r_anal_class_base_set (core->anal, cstr, &base);
		free (base.id);
		free (cstr);
		break;
	}
	default:
		r_core_cmd_help (core, help_msg_ac);
		break;
	}

	if (err == R_ANAL_CLASS_ERR_NONEXISTENT_CLASS) {
		eprintf ("Class does not exist.\n");
	}
}