static void kvmgt_dma_unmap_guest_page(unsigned long handle, dma_addr_t dma_addr)
{
	struct kvmgt_guest_info *info;
	struct gvt_dma *entry;

	if (!handle_valid(handle))
		return;

	info = (struct kvmgt_guest_info *)handle;

	mutex_lock(&info->vgpu->vdev.cache_lock);
	entry = __gvt_cache_find_dma_addr(info->vgpu, dma_addr);
	if (entry)
		kref_put(&entry->ref, __gvt_dma_release);
	mutex_unlock(&info->vgpu->vdev.cache_lock);
}