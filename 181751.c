static void var_help(RCore *core, char ch) {
	switch (ch) {
	case 'b':
		r_core_cmd_help (core, help_msg_afvb);
		break;
	case 's':
		r_core_cmd_help (core, help_msg_afvs);
		break;
	case 'r':
		r_core_cmd_help (core, help_msg_afvr);
		break;
	case '?':
		r_core_cmd_help (core, help_msg_afv);
		break;
	default:
		eprintf ("See afv?, afvb?, afvr? and afvs?\n");
	}
}