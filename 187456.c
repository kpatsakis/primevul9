static void find_back_node_value(struct mdesc_handle *hp, u64 node,
				 char *srch_val,
				 void (*func)(struct mdesc_handle *, u64, int),
				 u64 val, int depth)
{
	u64 arc;

	/* Since we have an estimate of recursion depth, do a sanity check. */
	if (depth == 0)
		return;

	mdesc_for_each_arc(arc, hp, node, MDESC_ARC_TYPE_BACK) {
		u64 n = mdesc_arc_target(hp, arc);
		const char *name = mdesc_node_name(hp, n);

		if (!strcmp(srch_val, name))
			(*func)(hp, n, val);

		find_back_node_value(hp, n, srch_val, func, val, depth-1);
	}
}