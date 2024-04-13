static void cmd_anal_rtti(RCore *core, const char *input) {
	switch (input[0]) {
	case '\0': // "avr"
	case 'j': // "avrj"
		r_anal_rtti_print_at_vtable (core->anal, core->offset, input[0]);
		break;
	case 'a': // "avra"
		r_anal_rtti_print_all (core->anal, input[1]);
		break;
	case 'r': // "avrr"
		r_anal_rtti_recover_all (core->anal);
		break;
	case 'D': { // "avrD"
		char *name = r_str_trim_dup (input + 1);
		char *demangled = r_anal_rtti_demangle_class_name (core->anal, name);
		free (name);
		if (demangled) {
			r_cons_println (demangled);
			free (demangled);
		}
		break;
	}
	default :
		r_core_cmd_help (core, help_msg_av);
		break;
	}
}