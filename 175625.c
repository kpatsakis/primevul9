static void __gvt_cache_remove_entry(struct intel_vgpu *vgpu,
				struct gvt_dma *entry)
{
	rb_erase(&entry->gfn_node, &vgpu->vdev.gfn_cache);
	rb_erase(&entry->dma_addr_node, &vgpu->vdev.dma_addr_cache);
	kfree(entry);
	vgpu->vdev.nr_cache_entries--;
}