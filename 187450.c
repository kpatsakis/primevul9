static void invoke_on_missing(const char *name,
			      struct mdesc_handle *a,
			      struct mdesc_handle *b,
			      void (*func)(struct mdesc_handle *, u64,
					   const char *node_name))
{
	mdesc_node_info_get_f get_info_func;
	mdesc_node_info_rel_f rel_info_func;
	mdesc_node_match_f node_match_func;
	union md_node_info a_node_info;
	union md_node_info b_node_info;
	bool found;
	u64 a_node;
	u64 b_node;
	int rv;

	/*
	 * Find the get_info, rel_info and node_match ops for the given
	 * node name
	 */
	mdesc_get_node_ops(name, &get_info_func, &rel_info_func,
			   &node_match_func);

	/* If we didn't find a match, the node type is not supported */
	if (!get_info_func || !rel_info_func || !node_match_func) {
		pr_err("MD: %s node type is not supported\n", name);
		return;
	}

	mdesc_for_each_node_by_name(a, a_node, name) {
		found = false;

		rv = get_info_func(a, a_node, &a_node_info);
		if (rv != 0) {
			pr_err("MD: Cannot find 1 or more required match properties for %s node.\n",
			       name);
			continue;
		}

		/* Check each node in B for node matching a_node */
		mdesc_for_each_node_by_name(b, b_node, name) {
			rv = get_info_func(b, b_node, &b_node_info);
			if (rv != 0)
				continue;

			if (node_match_func(&a_node_info, &b_node_info)) {
				found = true;
				rel_info_func(&b_node_info);
				break;
			}

			rel_info_func(&b_node_info);
		}

		rel_info_func(&a_node_info);

		if (!found)
			func(a, a_node, name);
	}
}