static int wcd9335_irq_init(struct wcd9335_codec *wcd)
{
	int ret;

	/*
	 * INTR1 consists of all possible interrupt sources Ear OCP,
	 * HPH OCP, MBHC, MAD, VBAT, and SVA
	 * INTR2 is a subset of first interrupt sources MAD, VBAT, and SVA
	 */
	wcd->intr1 = of_irq_get_byname(wcd->dev->of_node, "intr1");
	if (wcd->intr1 < 0) {
		if (wcd->intr1 != -EPROBE_DEFER)
			dev_err(wcd->dev, "Unable to configure IRQ\n");

		return wcd->intr1;
	}

	ret = devm_regmap_add_irq_chip(wcd->dev, wcd->regmap, wcd->intr1,
				 IRQF_TRIGGER_HIGH, 0,
				 &wcd9335_regmap_irq1_chip, &wcd->irq_data);
	if (ret)
		dev_err(wcd->dev, "Failed to register IRQ chip: %d\n", ret);

	return ret;
}