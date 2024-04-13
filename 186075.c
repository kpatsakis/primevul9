static int unittest_i2c_bus_probe(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	struct device_node *np = dev->of_node;
	struct unittest_i2c_bus_data *std;
	struct i2c_adapter *adap;
	int ret;

	if (np == NULL) {
		dev_err(dev, "No OF data for device\n");
		return -EINVAL;

	}

	dev_dbg(dev, "%s for node @%pOF\n", __func__, np);

	std = devm_kzalloc(dev, sizeof(*std), GFP_KERNEL);
	if (!std)
		return -ENOMEM;

	/* link them together */
	std->pdev = pdev;
	platform_set_drvdata(pdev, std);

	adap = &std->adap;
	i2c_set_adapdata(adap, std);
	adap->nr = -1;
	strlcpy(adap->name, pdev->name, sizeof(adap->name));
	adap->class = I2C_CLASS_DEPRECATED;
	adap->algo = &unittest_i2c_algo;
	adap->dev.parent = dev;
	adap->dev.of_node = dev->of_node;
	adap->timeout = 5 * HZ;
	adap->retries = 3;

	ret = i2c_add_numbered_adapter(adap);
	if (ret != 0) {
		dev_err(dev, "Failed to add I2C adapter\n");
		return ret;
	}

	return 0;
}