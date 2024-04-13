static int unittest_i2c_dev_remove(struct i2c_client *client)
{
	struct device *dev = &client->dev;
	struct device_node *np = client->dev.of_node;

	dev_dbg(dev, "%s for node @%pOF\n", __func__, np);
	return 0;
}