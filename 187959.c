static int has_fsl_hypervisor(void)
{
	struct device_node *node;
	int ret;

	node = of_find_node_by_path("/hypervisor");
	if (!node)
		return 0;

	ret = of_find_property(node, "fsl,hv-version", NULL) != NULL;

	of_node_put(node);

	return ret;
}