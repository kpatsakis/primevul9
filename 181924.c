static int wcd9335_slim_status(struct slim_device *sdev,
			       enum slim_device_status status)
{
	struct device *dev = &sdev->dev;
	struct device_node *ifc_dev_np;
	struct wcd9335_codec *wcd;
	int ret;

	wcd = dev_get_drvdata(dev);

	ifc_dev_np = of_parse_phandle(dev->of_node, "slim-ifc-dev", 0);
	if (!ifc_dev_np) {
		dev_err(dev, "No Interface device found\n");
		return -EINVAL;
	}

	wcd->slim = sdev;
	wcd->slim_ifc_dev = of_slim_get_device(sdev->ctrl, ifc_dev_np);
	of_node_put(ifc_dev_np);
	if (!wcd->slim_ifc_dev) {
		dev_err(dev, "Unable to get SLIM Interface device\n");
		return -EINVAL;
	}

	slim_get_logical_addr(wcd->slim_ifc_dev);

	wcd->regmap = regmap_init_slimbus(sdev, &wcd9335_regmap_config);
	if (IS_ERR(wcd->regmap)) {
		dev_err(dev, "Failed to allocate slim register map\n");
		return PTR_ERR(wcd->regmap);
	}

	wcd->if_regmap = regmap_init_slimbus(wcd->slim_ifc_dev,
						  &wcd9335_ifc_regmap_config);
	if (IS_ERR(wcd->if_regmap)) {
		dev_err(dev, "Failed to allocate ifc register map\n");
		return PTR_ERR(wcd->if_regmap);
	}

	ret = wcd9335_bring_up(wcd);
	if (ret) {
		dev_err(dev, "Failed to bringup WCD9335\n");
		return ret;
	}

	ret = wcd9335_irq_init(wcd);
	if (ret)
		return ret;

	wcd9335_probe(wcd);

	return ret;
}