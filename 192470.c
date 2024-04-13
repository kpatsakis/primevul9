static int fsl_lpspi_dma_transfer(struct spi_controller *controller,
				struct fsl_lpspi_data *fsl_lpspi,
				struct spi_transfer *transfer)
{
	struct dma_async_tx_descriptor *desc_tx, *desc_rx;
	unsigned long transfer_timeout;
	unsigned long timeout;
	struct sg_table *tx = &transfer->tx_sg, *rx = &transfer->rx_sg;
	int ret;

	ret = fsl_lpspi_dma_configure(controller);
	if (ret)
		return ret;

	desc_rx = dmaengine_prep_slave_sg(controller->dma_rx,
				rx->sgl, rx->nents, DMA_DEV_TO_MEM,
				DMA_PREP_INTERRUPT | DMA_CTRL_ACK);
	if (!desc_rx)
		return -EINVAL;

	desc_rx->callback = fsl_lpspi_dma_rx_callback;
	desc_rx->callback_param = (void *)fsl_lpspi;
	dmaengine_submit(desc_rx);
	reinit_completion(&fsl_lpspi->dma_rx_completion);
	dma_async_issue_pending(controller->dma_rx);

	desc_tx = dmaengine_prep_slave_sg(controller->dma_tx,
				tx->sgl, tx->nents, DMA_MEM_TO_DEV,
				DMA_PREP_INTERRUPT | DMA_CTRL_ACK);
	if (!desc_tx) {
		dmaengine_terminate_all(controller->dma_tx);
		return -EINVAL;
	}

	desc_tx->callback = fsl_lpspi_dma_tx_callback;
	desc_tx->callback_param = (void *)fsl_lpspi;
	dmaengine_submit(desc_tx);
	reinit_completion(&fsl_lpspi->dma_tx_completion);
	dma_async_issue_pending(controller->dma_tx);

	fsl_lpspi->slave_aborted = false;

	if (!fsl_lpspi->is_slave) {
		transfer_timeout = fsl_lpspi_calculate_timeout(fsl_lpspi,
							       transfer->len);

		/* Wait eDMA to finish the data transfer.*/
		timeout = wait_for_completion_timeout(&fsl_lpspi->dma_tx_completion,
						      transfer_timeout);
		if (!timeout) {
			dev_err(fsl_lpspi->dev, "I/O Error in DMA TX\n");
			dmaengine_terminate_all(controller->dma_tx);
			dmaengine_terminate_all(controller->dma_rx);
			fsl_lpspi_reset(fsl_lpspi);
			return -ETIMEDOUT;
		}

		timeout = wait_for_completion_timeout(&fsl_lpspi->dma_rx_completion,
						      transfer_timeout);
		if (!timeout) {
			dev_err(fsl_lpspi->dev, "I/O Error in DMA RX\n");
			dmaengine_terminate_all(controller->dma_tx);
			dmaengine_terminate_all(controller->dma_rx);
			fsl_lpspi_reset(fsl_lpspi);
			return -ETIMEDOUT;
		}
	} else {
		if (wait_for_completion_interruptible(&fsl_lpspi->dma_tx_completion) ||
			fsl_lpspi->slave_aborted) {
			dev_dbg(fsl_lpspi->dev,
				"I/O Error in DMA TX interrupted\n");
			dmaengine_terminate_all(controller->dma_tx);
			dmaengine_terminate_all(controller->dma_rx);
			fsl_lpspi_reset(fsl_lpspi);
			return -EINTR;
		}

		if (wait_for_completion_interruptible(&fsl_lpspi->dma_rx_completion) ||
			fsl_lpspi->slave_aborted) {
			dev_dbg(fsl_lpspi->dev,
				"I/O Error in DMA RX interrupted\n");
			dmaengine_terminate_all(controller->dma_tx);
			dmaengine_terminate_all(controller->dma_rx);
			fsl_lpspi_reset(fsl_lpspi);
			return -EINTR;
		}
	}

	fsl_lpspi_reset(fsl_lpspi);

	return 0;
}