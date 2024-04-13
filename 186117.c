static int unittest_i2c_bus_remove(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	struct device_node *np = dev->of_node;
	struct unittest_i2c_bus_data *std = platform_get_drvdata(pdev);

	dev_dbg(dev, "%s for node @%pOF\n", __func__, np);
	i2c_del_adapter(&std->adap);

	return 0;
}