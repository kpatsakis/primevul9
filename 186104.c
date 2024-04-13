static int unittest_remove(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	struct device_node *np = dev->of_node;

	dev_dbg(dev, "%s for node @%pOF\n", __func__, np);
	return 0;
}