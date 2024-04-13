static void gvt_dma_unmap_page(struct intel_vgpu *vgpu, unsigned long gfn,
		dma_addr_t dma_addr, unsigned long size)
{
	struct device *dev = &vgpu->gvt->dev_priv->drm.pdev->dev;

	dma_unmap_page(dev, dma_addr, size, PCI_DMA_BIDIRECTIONAL);
	gvt_unpin_guest_page(vgpu, gfn, size);
}