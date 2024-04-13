static void gvt_cache_init(struct intel_vgpu *vgpu)
{
	vgpu->vdev.gfn_cache = RB_ROOT;
	vgpu->vdev.dma_addr_cache = RB_ROOT;
	vgpu->vdev.nr_cache_entries = 0;
	mutex_init(&vgpu->vdev.cache_lock);
}