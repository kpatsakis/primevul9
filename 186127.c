static int unittest_i2c_mux_remove(struct i2c_client *client)
{
	struct device *dev = &client->dev;
	struct device_node *np = client->dev.of_node;
	struct i2c_mux_core *muxc = i2c_get_clientdata(client);

	dev_dbg(dev, "%s for node @%pOF\n", __func__, np);
	i2c_mux_del_adapters(muxc);
	return 0;
}