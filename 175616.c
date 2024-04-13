static int kvmgt_dma_map_guest_page(unsigned long handle, unsigned long gfn,
		unsigned long size, dma_addr_t *dma_addr)
{
	struct kvmgt_guest_info *info;
	struct intel_vgpu *vgpu;
	struct gvt_dma *entry;
	int ret;

	if (!handle_valid(handle))
		return -EINVAL;

	info = (struct kvmgt_guest_info *)handle;
	vgpu = info->vgpu;

	mutex_lock(&info->vgpu->vdev.cache_lock);

	entry = __gvt_cache_find_gfn(info->vgpu, gfn);
	if (!entry) {
		ret = gvt_dma_map_page(vgpu, gfn, dma_addr, size);
		if (ret)
			goto err_unlock;

		ret = __gvt_cache_add(info->vgpu, gfn, *dma_addr, size);
		if (ret)
			goto err_unmap;
	} else {
		kref_get(&entry->ref);
		*dma_addr = entry->dma_addr;
	}

	mutex_unlock(&info->vgpu->vdev.cache_lock);
	return 0;

err_unmap:
	gvt_dma_unmap_page(vgpu, gfn, *dma_addr, size);
err_unlock:
	mutex_unlock(&info->vgpu->vdev.cache_lock);
	return ret;
}