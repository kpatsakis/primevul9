R_API RGraph *r_core_anal_importxrefs(RCore *core) {
	RBinInfo *info = r_bin_get_info (core->bin);
	RBinObject *obj = r_bin_cur_object (core->bin);
	bool lit = info? info->has_lit: false;
	bool va = core->io->va || r_config_get_b (core->config, "cfg.debug");

	RListIter *iter;
	RBinImport *imp;
	if (!obj) {
		return NULL;
	}
	RGraph *graph = r_graph_new ();
	if (!graph) {
		return NULL;
	}
	r_list_foreach (obj->imports, iter, imp) {
		ut64 addr = lit ? r_core_bin_impaddr (core->bin, va, imp->name): 0;
		if (addr) {
			add_single_addr_xrefs (core, addr, graph);
		} else {
			r_graph_add_node_info (graph, imp->name, NULL, 0);
		}
	}
	return graph;
}