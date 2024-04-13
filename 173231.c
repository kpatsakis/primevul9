static int bnx2x_set_coherency_mask(struct bnx2x *bp)
{
	struct device *dev = &bp->pdev->dev;

	if (dma_set_mask_and_coherent(dev, DMA_BIT_MASK(64)) != 0 &&
	    dma_set_mask_and_coherent(dev, DMA_BIT_MASK(32)) != 0) {
		dev_err(dev, "System does not support DMA, aborting\n");
		return -EIO;
	}

	return 0;
}