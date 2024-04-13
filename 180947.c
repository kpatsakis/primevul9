static void build_zonelists(pg_data_t *pgdat)
{
	static int node_order[MAX_NUMNODES];
	int node, load, nr_nodes = 0;
	nodemask_t used_mask;
	int local_node, prev_node;

	/* NUMA-aware ordering of nodes */
	local_node = pgdat->node_id;
	load = nr_online_nodes;
	prev_node = local_node;
	nodes_clear(used_mask);

	memset(node_order, 0, sizeof(node_order));
	while ((node = find_next_best_node(local_node, &used_mask)) >= 0) {
		/*
		 * We don't want to pressure a particular node.
		 * So adding penalty to the first node in same
		 * distance group to make it round-robin.
		 */
		if (node_distance(local_node, node) !=
		    node_distance(local_node, prev_node))
			node_load[node] = load;

		node_order[nr_nodes++] = node;
		prev_node = node;
		load--;
	}

	build_zonelists_in_node_order(pgdat, node_order, nr_nodes);
	build_thisnode_zonelists(pgdat);
}