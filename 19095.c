static void add_single_addr_xrefs(RCore *core, ut64 addr, RGraph *graph) {
	r_return_if_fail (graph);
	RFlagItem *f = r_flag_get_at (core->flags, addr, false);
	char *me = (f && f->offset == addr)
		? r_str_new (f->name)
		: r_str_newf ("0x%" PFMT64x, addr);

	RGraphNode *curr_node = r_graph_add_node_info (graph, me, NULL, addr);
	R_FREE (me);
	if (!curr_node) {
		return;
	}
	RListIter *iter;
	RAnalRef *ref;
	RList *list = r_anal_xrefs_get (core->anal, addr);
	r_list_foreach (list, iter, ref) {
		RFlagItem *item = r_flag_get_i (core->flags, ref->addr);
		char *src = item? r_str_new (item->name): r_str_newf ("0x%08" PFMT64x, ref->addr);
		RGraphNode *reference_from = r_graph_add_node_info (graph, src, NULL, ref->addr);
		free (src);
		r_graph_add_edge (graph, reference_from, curr_node);
	}
	r_list_free (list);
}