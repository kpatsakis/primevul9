static int wcd9335_slim_probe(struct slim_device *slim)
{
	struct device *dev = &slim->dev;
	struct wcd9335_codec *wcd;
	int ret;

	wcd = devm_kzalloc(dev, sizeof(*wcd), GFP_KERNEL);
	if (!wcd)
		return	-ENOMEM;

	wcd->dev = dev;
	ret = wcd9335_parse_dt(wcd);
	if (ret) {
		dev_err(dev, "Error parsing DT: %d\n", ret);
		return ret;
	}

	ret = wcd9335_power_on_reset(wcd);
	if (ret)
		return ret;

	dev_set_drvdata(dev, wcd);

	return 0;
}