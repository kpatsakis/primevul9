static int spi_gpio_probe_pdata(struct platform_device *pdev,
				struct spi_master *master)
{
	struct device *dev = &pdev->dev;
	struct spi_gpio_platform_data *pdata = dev_get_platdata(dev);
	struct spi_gpio *spi_gpio = spi_master_get_devdata(master);
	int i;

#ifdef GENERIC_BITBANG
	if (!pdata || !pdata->num_chipselect)
		return -ENODEV;
#endif
	/*
	 * The master needs to think there is a chipselect even if not
	 * connected
	 */
	master->num_chipselect = pdata->num_chipselect ?: 1;

	spi_gpio->cs_gpios = devm_kcalloc(dev, master->num_chipselect,
					  sizeof(*spi_gpio->cs_gpios),
					  GFP_KERNEL);
	if (!spi_gpio->cs_gpios)
		return -ENOMEM;

	for (i = 0; i < master->num_chipselect; i++) {
		spi_gpio->cs_gpios[i] = devm_gpiod_get_index(dev, "cs", i,
							     GPIOD_OUT_HIGH);
		if (IS_ERR(spi_gpio->cs_gpios[i]))
			return PTR_ERR(spi_gpio->cs_gpios[i]);
	}

	return 0;
}