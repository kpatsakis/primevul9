static void gvt_cache_destroy(struct intel_vgpu *vgpu)
{
	struct gvt_dma *dma;
	struct rb_node *node = NULL;

	for (;;) {
		mutex_lock(&vgpu->vdev.cache_lock);
		node = rb_first(&vgpu->vdev.gfn_cache);
		if (!node) {
			mutex_unlock(&vgpu->vdev.cache_lock);
			break;
		}
		dma = rb_entry(node, struct gvt_dma, gfn_node);
		gvt_dma_unmap_page(vgpu, dma->gfn, dma->dma_addr, dma->size);
		__gvt_cache_remove_entry(vgpu, dma);
		mutex_unlock(&vgpu->vdev.cache_lock);
	}
}