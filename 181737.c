static void cmd_anal_class_method(RCore *core, const char *input) {
	RAnalClassErr err = R_ANAL_CLASS_ERR_SUCCESS;
	char c = input[0];
	switch (c) {
	case ' ': // "acm"
	case '-': // "acm-"
	case 'n': { // "acmn"
		const char *str = r_str_trim_head_ro (input + 1);
		if (!*str) {
			eprintf ("No class name given.\n");
			break;
		}
		char *cstr = strdup (str);
		if (!cstr) {
			break;
		}
		char *end = strchr (cstr, ' ');
		if (!end) {
			eprintf ("No method name given.\n");
			free (cstr);
			break;
		}
		*end = '\0';
		char *name_str = end + 1;

		if (c == ' ' || c == 'n') {
			end = strchr (name_str, ' ');
			if (!end) {
				if (c == ' ') {
					eprintf ("No offset given.\n");
				} else if (c == 'n') {
					eprintf ("No new method name given.\n");
				}
				free (cstr);
				break;
			}
			*end = '\0';
		}

		if (c == ' ') {
			char *addr_str = end + 1;
			end = strchr (addr_str, ' ');
			if (end) {
				*end = '\0';
			}

			RAnalMethod meth;
			meth.name = name_str;
			meth.addr = r_num_get (core->num, addr_str);
			meth.vtable_offset = -1;
			if (end) {
				meth.vtable_offset = (int)r_num_get (core->num, end + 1);
			}
			err = r_anal_class_method_set (core->anal, cstr, &meth);
		} else if (c == 'n') {
			char *new_name_str = end + 1;
			end = strchr (new_name_str, ' ');
			if (end) {
				*end = '\0';
			}

			err = r_anal_class_method_rename (core->anal, cstr, name_str, new_name_str);
		} else if (c == '-') {
			err = r_anal_class_method_delete (core->anal, cstr, name_str);
		}

		free (cstr);
		break;
	}
	default:
		r_core_cmd_help (core, help_msg_ac);
		break;
	}

	switch (err) {
		case R_ANAL_CLASS_ERR_NONEXISTENT_CLASS:
			eprintf ("Class does not exist.\n");
			break;
		case R_ANAL_CLASS_ERR_NONEXISTENT_ATTR:
			eprintf ("Method does not exist.\n");
			break;
		default:
			break;
	}
}