int mdesc_get_node_info(struct mdesc_handle *hp, u64 node,
			const char *node_name, union md_node_info *node_info)
{
	mdesc_node_info_get_f get_info_func;
	int rv;

	if (hp == NULL || node == MDESC_NODE_NULL ||
	    node_name == NULL || node_info == NULL)
		return -EINVAL;

	/* Find the get_info op for the given node name */
	mdesc_get_node_ops(node_name, &get_info_func, NULL, NULL);

	/* If we didn't find a get_info_func, the node name is not supported */
	if (get_info_func == NULL) {
		pr_err("MD: %s node is not supported\n", node_name);
		return -EINVAL;
	}

	rv = get_info_func(hp, node, node_info);
	if (rv != 0) {
		pr_err("MD: Cannot find 1 or more required match properties for %s node.\n",
		       node_name);
		return -1;
	}

	return 0;
}