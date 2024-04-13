static inline void sdma_unmap_desc(
	struct hfi1_devdata *dd,
	struct sdma_desc *descp)
{
	switch (sdma_mapping_type(descp)) {
	case SDMA_MAP_SINGLE:
		dma_unmap_single(
			&dd->pcidev->dev,
			sdma_mapping_addr(descp),
			sdma_mapping_len(descp),
			DMA_TO_DEVICE);
		break;
	case SDMA_MAP_PAGE:
		dma_unmap_page(
			&dd->pcidev->dev,
			sdma_mapping_addr(descp),
			sdma_mapping_len(descp),
			DMA_TO_DEVICE);
		break;
	}
}