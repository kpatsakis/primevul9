static int wcd9335_setup_irqs(struct wcd9335_codec *wcd)
{
	int irq, ret, i;

	for (i = 0; i < ARRAY_SIZE(wcd9335_irqs); i++) {
		irq = regmap_irq_get_virq(wcd->irq_data, wcd9335_irqs[i].irq);
		if (irq < 0) {
			dev_err(wcd->dev, "Failed to get %s\n",
					wcd9335_irqs[i].name);
			return irq;
		}

		ret = devm_request_threaded_irq(wcd->dev, irq, NULL,
						wcd9335_irqs[i].handler,
						IRQF_TRIGGER_RISING,
						wcd9335_irqs[i].name, wcd);
		if (ret) {
			dev_err(wcd->dev, "Failed to request %s\n",
					wcd9335_irqs[i].name);
			return ret;
		}
	}

	/* enable interrupts on all slave ports */
	for (i = 0; i < WCD9335_SLIM_NUM_PORT_REG; i++)
		regmap_write(wcd->if_regmap, WCD9335_SLIM_PGD_PORT_INT_EN0 + i,
			     0xFF);

	return ret;
}