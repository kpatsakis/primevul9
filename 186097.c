static int unittest_i2c_mux_probe(struct i2c_client *client,
		const struct i2c_device_id *id)
{
	int i, nchans;
	struct device *dev = &client->dev;
	struct i2c_adapter *adap = client->adapter;
	struct device_node *np = client->dev.of_node, *child;
	struct i2c_mux_core *muxc;
	u32 reg, max_reg;

	dev_dbg(dev, "%s for node @%pOF\n", __func__, np);

	if (!np) {
		dev_err(dev, "No OF node\n");
		return -EINVAL;
	}

	max_reg = (u32)-1;
	for_each_child_of_node(np, child) {
		if (of_property_read_u32(child, "reg", &reg))
			continue;
		if (max_reg == (u32)-1 || reg > max_reg)
			max_reg = reg;
	}
	nchans = max_reg == (u32)-1 ? 0 : max_reg + 1;
	if (nchans == 0) {
		dev_err(dev, "No channels\n");
		return -EINVAL;
	}

	muxc = i2c_mux_alloc(adap, dev, nchans, 0, 0,
			     unittest_i2c_mux_select_chan, NULL);
	if (!muxc)
		return -ENOMEM;
	for (i = 0; i < nchans; i++) {
		if (i2c_mux_add_adapter(muxc, 0, i, 0)) {
			dev_err(dev, "Failed to register mux #%d\n", i);
			i2c_mux_del_adapters(muxc);
			return -ENODEV;
		}
	}

	i2c_set_clientdata(client, muxc);

	return 0;
};