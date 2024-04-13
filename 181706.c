static void cmd_anal_class_vtable(RCore *core, const char *input) {
	RAnalClassErr err = R_ANAL_CLASS_ERR_SUCCESS;
	char c = input[0];
	switch (c) {
	case 'f': {// "acvf" [offset] ([class_name])
		const char *str = r_str_trim_head_ro (input + 1);
		if (!*str) {
			eprintf ("No offset given\n");
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
		ut64 offset_arg = r_num_get (core->num, cstr); // Should I allow negative offset?
		char *class_arg = NULL;
		if (end) {
			class_arg = (char *)r_str_trim_head_ro (end);
		}

		if (class_arg) {
			end = (char *)r_str_trim_head_wp (class_arg); // in case of extra unwanted stuff at the cmd end
			*end = '\0';
		}
		r_anal_class_list_vtable_offset_functions (core->anal, class_arg, offset_arg);

		free (cstr);
		break;
	}
	case ' ': // "acv"
	case '-': { // "acv-"
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
				r_anal_class_list_vtables (core->anal, cstr);
			} else /*if (c == '-')*/ {
				eprintf ("No vtable id given. See acv [class name].\n");
			}
			free (cstr);
			break;
		}

		char *arg1_str = end;

		if (c == '-') {
			err = r_anal_class_vtable_delete (core->anal, cstr, arg1_str);
			free (cstr);
			break;
		}

		end = strchr (arg1_str, ' ');
		if (end) {
			*end = '\0';
		}
		
		RAnalVTable vtable;
		vtable.id = NULL;
		vtable.addr = r_num_get (core->num, arg1_str);
		vtable.offset = 0;
		vtable.size = 0;

		char *arg3_str = NULL;
		if (end) {
			vtable.offset = r_num_get (core->num, end + 1);
			// end + 1 won't work on extra whitespace between arguments, TODO
			arg3_str = strchr (end+1, ' ');
		}

		if (arg3_str) {
			vtable.size = r_num_get (core->num, arg3_str + 1);
		}

		err = r_anal_class_vtable_set (core->anal, cstr, &vtable);
		free (vtable.id);
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