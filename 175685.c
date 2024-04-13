static int gvt_dma_map_page(struct intel_vgpu *vgpu, unsigned long gfn,
		dma_addr_t *dma_addr, unsigned long size)
{
	struct device *dev = &vgpu->gvt->dev_priv->drm.pdev->dev;
	struct page *page = NULL;
	int ret;

	ret = gvt_pin_guest_page(vgpu, gfn, size, &page);
	if (ret)
		return ret;

	/* Setup DMA mapping. */
	*dma_addr = dma_map_page(dev, page, 0, size, PCI_DMA_BIDIRECTIONAL);
	if (dma_mapping_error(dev, *dma_addr)) {
		gvt_vgpu_err("DMA mapping failed for pfn 0x%lx, ret %d\n",
			     page_to_pfn(page), ret);
		gvt_unpin_guest_page(vgpu, gfn, size);
		return -ENOMEM;
	}

	return 0;
}