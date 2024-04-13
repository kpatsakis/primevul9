static int fsl_lpspi_dma_configure(struct spi_controller *controller)
{
	int ret;
	enum dma_slave_buswidth buswidth;
	struct dma_slave_config rx = {}, tx = {};
	struct fsl_lpspi_data *fsl_lpspi =
				spi_controller_get_devdata(controller);

	switch (fsl_lpspi_bytes_per_word(fsl_lpspi->config.bpw)) {
	case 4:
		buswidth = DMA_SLAVE_BUSWIDTH_4_BYTES;
		break;
	case 2:
		buswidth = DMA_SLAVE_BUSWIDTH_2_BYTES;
		break;
	case 1:
		buswidth = DMA_SLAVE_BUSWIDTH_1_BYTE;
		break;
	default:
		return -EINVAL;
	}

	tx.direction = DMA_MEM_TO_DEV;
	tx.dst_addr = fsl_lpspi->base_phys + IMX7ULP_TDR;
	tx.dst_addr_width = buswidth;
	tx.dst_maxburst = 1;
	ret = dmaengine_slave_config(controller->dma_tx, &tx);
	if (ret) {
		dev_err(fsl_lpspi->dev, "TX dma configuration failed with %d\n",
			ret);
		return ret;
	}

	rx.direction = DMA_DEV_TO_MEM;
	rx.src_addr = fsl_lpspi->base_phys + IMX7ULP_RDR;
	rx.src_addr_width = buswidth;
	rx.src_maxburst = 1;
	ret = dmaengine_slave_config(controller->dma_rx, &rx);
	if (ret) {
		dev_err(fsl_lpspi->dev, "RX dma configuration failed with %d\n",
			ret);
		return ret;
	}

	return 0;
}