static bool anal_bb_edge (RCore *core, const char *input) {
	// "afbe" switch-bb-addr case-bb-addr
	char *arg = strdup (r_str_trim_head_ro (input));
	char *sp = strchr (arg, ' ');
	bool ret = false;
	if (sp) {
		*sp++ = 0;
		ut64 switch_addr = r_num_math (core->num, arg);
		ut64 case_addr = r_num_math (core->num, sp);
		RList *blocks = r_anal_get_blocks_in (core->anal, switch_addr);
		if (blocks && !r_list_empty (blocks)) {
			r_anal_block_add_switch_case (r_list_first (blocks), switch_addr, case_addr);
			ret = true;
		}
		r_list_free (blocks);
	}
	free (arg);
	return ret;
}