static void cmd_anal_virtual_functions(RCore *core, const char* input) {
	switch (input[0]) {
	case '\0': // "av"
	case '*': // "av*"
	case 'j': // "avj"
		r_anal_list_vtables (core->anal, input[0]);
		break;
	case 'r': // "avr"
		cmd_anal_rtti (core, input + 1);
		break;
	default :
		r_core_cmd_help (core, help_msg_av);
		break;
	}
}