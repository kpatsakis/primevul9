R_API RList* r_core_anal_graph_to(RCore *core, ut64 addr, int n) {
	int depth = r_config_get_i (core->config, "anal.graph_depth");
	RList *path, *paths = r_list_new ();
	HtUP *avoid = ht_up_new0 ();
	while (n) {
		path = anal_graph_to (core, addr, depth, avoid);
		if (path) {
			r_list_append (paths, path);
			if (r_list_length (path) >= 2) {
				RAnalBlock *last = r_list_get_n (path, r_list_length (path) - 2);
				ht_up_update (avoid, last->addr, last);
				n--;
				continue;
			}
		}
		// no more path found
		break;
	}
	ht_up_free (avoid);
	return paths;
}