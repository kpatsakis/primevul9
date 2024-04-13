static struct gvt_dma *__gvt_cache_find_dma_addr(struct intel_vgpu *vgpu,
		dma_addr_t dma_addr)
{
	struct rb_node *node = vgpu->vdev.dma_addr_cache.rb_node;
	struct gvt_dma *itr;

	while (node) {
		itr = rb_entry(node, struct gvt_dma, dma_addr_node);

		if (dma_addr < itr->dma_addr)
			node = node->rb_left;
		else if (dma_addr > itr->dma_addr)
			node = node->rb_right;
		else
			return itr;
	}
	return NULL;
}