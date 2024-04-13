static int fcn_list_verbose(RCore *core, RList *fcns, const char *sortby) {
	bool use_color = r_config_get_i (core->config, "scr.color");
	int headeraddr_width = 10;
	char *headeraddr = "==========";

	if (core->anal->bits == 64) {
		headeraddr_width = 18;
		headeraddr = "==================";
	}

	if (sortby) {
		if (!strcmp (sortby, "size")) {
			r_list_sort (fcns, cmpsize);
		} else if (!strcmp (sortby, "addr")) {
			r_list_sort (fcns, cmpaddr);
		} else if (!strcmp (sortby, "cc")) {
			r_list_sort (fcns, cmpfcncc);
		} else if (!strcmp (sortby, "edges")) {
			r_list_sort (fcns, cmpedges);
		} else if (!strcmp (sortby, "calls")) {
			r_list_sort (fcns, cmpcalls);
		} else if (strstr (sortby, "name")) {
			r_list_sort (fcns, cmpname);
		} else if (strstr (sortby, "frame")) {
			r_list_sort (fcns, cmpframe);
		} else if (strstr (sortby, "ref")) {
			r_list_sort (fcns, cmpxrefs);
		} else if (!strcmp (sortby, "nbbs")) {
			r_list_sort (fcns, cmpnbbs);
		}
	}

	r_cons_printf ("%-*s %4s %5s %5s %5s %4s %*s range %-*s %s %s %s %s %s %s\n",
			headeraddr_width, "address", "size", "nbbs", "edges", "cc", "cost",
			headeraddr_width, "min bound", headeraddr_width, "max bound", "calls",
			"locals", "args", "xref", "frame", "name");
	r_cons_printf ("%s ==== ===== ===== ===== ==== %s ===== %s ===== ====== ==== ==== ===== ====\n",
			headeraddr, headeraddr, headeraddr);
	RListIter *iter;
	RAnalFunction *fcn;
	r_list_foreach (fcns, iter, fcn) {
		fcn_print_verbose (core, fcn, use_color);
	}

	return 0;
}