static int spi_gpio_probe_dt(struct platform_device *pdev,
			     struct spi_master *master)
{
	master->dev.of_node = pdev->dev.of_node;
	master->use_gpio_descriptors = true;

	return 0;
}