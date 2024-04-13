static void __gvt_dma_release(struct kref *ref)
{
	struct gvt_dma *entry = container_of(ref, typeof(*entry), ref);

	gvt_dma_unmap_page(entry->vgpu, entry->gfn, entry->dma_addr,
			   entry->size);
	__gvt_cache_remove_entry(entry->vgpu, entry);
}