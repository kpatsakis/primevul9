static struct i2c_client *of_path_to_i2c_client(const char *path)
{
	struct device_node *np;
	struct i2c_client *client;

	np = of_find_node_by_path(path);
	if (np == NULL)
		return NULL;

	client = of_find_i2c_device_by_node(np);
	of_node_put(np);

	return client;
}