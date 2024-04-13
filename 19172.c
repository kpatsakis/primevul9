static int fcn_print_verbose(RCore *core, RAnalFunction *fcn, bool use_color) {
	char *name = r_core_anal_fcn_name (core, fcn);
	int ebbs = 0;
	int addrwidth = 8;
	const char *color = "";
	const char *color_end = "";
	if (use_color) {
		color_end = Color_RESET;
		if (strstr (name, "sym.imp.")) {
			color = Color_YELLOW;
		} else if (strstr (name, "sym.")) {
			color = Color_GREEN;
		} else if (strstr (name, "sub.")) {
			color = Color_MAGENTA;
		}
	}

	if (core->anal->bits == 64) {
		addrwidth = 16;
	}

	r_cons_printf (FCN_LIST_VERBOSE_ENTRY, color,
			addrwidth, fcn->addr,
			r_anal_function_realsize (fcn),
			r_list_length (fcn->bbs),
			r_anal_function_count_edges (fcn, &ebbs),
			r_anal_function_complexity (fcn),
			r_anal_function_cost (fcn),
			addrwidth, r_anal_function_min_addr (fcn),
			r_anal_function_linear_size (fcn),
			addrwidth, r_anal_function_max_addr (fcn),
			fcn->meta.numcallrefs,
			r_anal_var_count_locals (fcn),
			r_anal_var_count_args (fcn),
			fcn->meta.numrefs,
			fcn->maxstack,
			name,
			color_end);
	free (name);
	return 0;
}