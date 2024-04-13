static int intel_vgpu_iommu_notifier(struct notifier_block *nb,
				     unsigned long action, void *data)
{
	struct intel_vgpu *vgpu = container_of(nb,
					struct intel_vgpu,
					vdev.iommu_notifier);

	if (action == VFIO_IOMMU_NOTIFY_DMA_UNMAP) {
		struct vfio_iommu_type1_dma_unmap *unmap = data;
		struct gvt_dma *entry;
		unsigned long iov_pfn, end_iov_pfn;

		iov_pfn = unmap->iova >> PAGE_SHIFT;
		end_iov_pfn = iov_pfn + unmap->size / PAGE_SIZE;

		mutex_lock(&vgpu->vdev.cache_lock);
		for (; iov_pfn < end_iov_pfn; iov_pfn++) {
			entry = __gvt_cache_find_gfn(vgpu, iov_pfn);
			if (!entry)
				continue;

			gvt_dma_unmap_page(vgpu, entry->gfn, entry->dma_addr,
					   entry->size);
			__gvt_cache_remove_entry(vgpu, entry);
		}
		mutex_unlock(&vgpu->vdev.cache_lock);
	}

	return NOTIFY_OK;
}