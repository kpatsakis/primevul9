u64 mdesc_get_node(struct mdesc_handle *hp, const char *node_name,
		   union md_node_info *node_info)
{
	mdesc_node_info_get_f get_info_func;
	mdesc_node_info_rel_f rel_info_func;
	mdesc_node_match_f node_match_func;
	union md_node_info hp_node_info;
	u64 hp_node;
	int rv;

	if (hp == NULL || node_name == NULL || node_info == NULL)
		return MDESC_NODE_NULL;

	/* Find the ops for the given node name */
	mdesc_get_node_ops(node_name, &get_info_func, &rel_info_func,
			   &node_match_func);

	/* If we didn't find ops for the given node name, it is not supported */
	if (!get_info_func || !rel_info_func || !node_match_func) {
		pr_err("MD: %s node is not supported\n", node_name);
		return -EINVAL;
	}

	mdesc_for_each_node_by_name(hp, hp_node, node_name) {
		rv = get_info_func(hp, hp_node, &hp_node_info);
		if (rv != 0)
			continue;

		if (node_match_func(node_info, &hp_node_info))
			break;

		rel_info_func(&hp_node_info);
	}

	rel_info_func(&hp_node_info);

	return hp_node;
}