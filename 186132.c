static int unittest_probe(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	struct device_node *np = dev->of_node;

	if (np == NULL) {
		dev_err(dev, "No OF data for device\n");
		return -EINVAL;

	}

	dev_dbg(dev, "%s for node @%pOF\n", __func__, np);

	of_platform_populate(np, NULL, NULL, &pdev->dev);

	return 0;
}