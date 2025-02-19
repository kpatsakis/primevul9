static int spi_gpio_probe(struct platform_device *pdev)
{
	int				status;
	struct spi_master		*master;
	struct spi_gpio			*spi_gpio;
	struct device			*dev = &pdev->dev;
	struct spi_bitbang		*bb;
	const struct of_device_id	*of_id;

	of_id = of_match_device(spi_gpio_dt_ids, &pdev->dev);

	master = spi_alloc_master(dev, sizeof(*spi_gpio));
	if (!master)
		return -ENOMEM;

	status = devm_add_action_or_reset(&pdev->dev, spi_gpio_put, master);
	if (status) {
		spi_master_put(master);
		return status;
	}

	if (of_id)
		status = spi_gpio_probe_dt(pdev, master);
	else
		status = spi_gpio_probe_pdata(pdev, master);

	if (status)
		return status;

	spi_gpio = spi_master_get_devdata(master);

	status = spi_gpio_request(dev, spi_gpio);
	if (status)
		return status;

	master->bits_per_word_mask = SPI_BPW_RANGE_MASK(1, 32);
	master->mode_bits = SPI_3WIRE | SPI_3WIRE_HIZ | SPI_CPHA | SPI_CPOL |
			    SPI_CS_HIGH;
	if (!spi_gpio->mosi) {
		/* HW configuration without MOSI pin
		 *
		 * No setting SPI_MASTER_NO_RX here - if there is only
		 * a MOSI pin connected the host can still do RX by
		 * changing the direction of the line.
		 */
		master->flags = SPI_MASTER_NO_TX;
	}

	master->bus_num = pdev->id;
	master->setup = spi_gpio_setup;
	master->cleanup = spi_gpio_cleanup;

	bb = &spi_gpio->bitbang;
	bb->master = master;
	/*
	 * There is some additional business, apart from driving the CS GPIO
	 * line, that we need to do on selection. This makes the local
	 * callback for chipselect always get called.
	 */
	master->flags |= SPI_MASTER_GPIO_SS;
	bb->chipselect = spi_gpio_chipselect;
	bb->set_line_direction = spi_gpio_set_direction;

	if (master->flags & SPI_MASTER_NO_TX) {
		bb->txrx_word[SPI_MODE_0] = spi_gpio_spec_txrx_word_mode0;
		bb->txrx_word[SPI_MODE_1] = spi_gpio_spec_txrx_word_mode1;
		bb->txrx_word[SPI_MODE_2] = spi_gpio_spec_txrx_word_mode2;
		bb->txrx_word[SPI_MODE_3] = spi_gpio_spec_txrx_word_mode3;
	} else {
		bb->txrx_word[SPI_MODE_0] = spi_gpio_txrx_word_mode0;
		bb->txrx_word[SPI_MODE_1] = spi_gpio_txrx_word_mode1;
		bb->txrx_word[SPI_MODE_2] = spi_gpio_txrx_word_mode2;
		bb->txrx_word[SPI_MODE_3] = spi_gpio_txrx_word_mode3;
	}
	bb->setup_transfer = spi_bitbang_setup_transfer;

	status = spi_bitbang_init(&spi_gpio->bitbang);
	if (status)
		return status;

	return devm_spi_register_master(&pdev->dev, spi_master_get(master));
}