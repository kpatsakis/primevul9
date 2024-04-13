static struct platform_device *of_path_to_platform_device(const char *path)
{
	struct device_node *np;
	struct platform_device *pdev;

	np = of_find_node_by_path(path);
	if (np == NULL)
		return NULL;

	pdev = of_find_device_by_node(np);
	of_node_put(np);

	return pdev;
}