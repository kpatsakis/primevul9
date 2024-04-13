R_API RGraph *r_core_anal_codexrefs(RCore *core, ut64 addr) {
	RGraph *graph = r_graph_new ();
	if (!graph) {
		return NULL;
	}
	add_single_addr_xrefs (core, addr, graph);
	return graph;
}