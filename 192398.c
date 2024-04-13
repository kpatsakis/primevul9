static inline dma_addr_t skb_frag_dma_map(struct device *dev,
					  const skb_frag_t *frag,
					  size_t offset, size_t size,
					  enum dma_data_direction dir)
{
	return dma_map_page(dev, skb_frag_page(frag),
			    frag->page_offset + offset, size, dir);
}