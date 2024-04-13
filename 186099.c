static int unittest_i2c_dev_probe(struct i2c_client *client,
		const struct i2c_device_id *id)
{
	struct device *dev = &client->dev;
	struct device_node *np = client->dev.of_node;

	if (!np) {
		dev_err(dev, "No OF node\n");
		return -EINVAL;
	}

	dev_dbg(dev, "%s for node @%pOF\n", __func__, np);

	return 0;
};