static void reloc_cache_reset(struct reloc_cache *cache)
{
	void *vaddr;

	if (cache->rq)
		reloc_gpu_flush(cache);

	if (!cache->vaddr)
		return;

	vaddr = unmask_page(cache->vaddr);
	if (cache->vaddr & KMAP) {
		if (cache->vaddr & CLFLUSH_AFTER)
			mb();

		kunmap_atomic(vaddr);
		i915_gem_obj_finish_shmem_access((struct drm_i915_gem_object *)cache->node.mm);
	} else {
		wmb();
		io_mapping_unmap_atomic((void __iomem *)vaddr);
		if (cache->node.allocated) {
			struct i915_ggtt *ggtt = cache_to_ggtt(cache);

			ggtt->vm.clear_range(&ggtt->vm,
					     cache->node.start,
					     cache->node.size);
			drm_mm_remove_node(&cache->node);
		} else {
			i915_vma_unpin((struct i915_vma *)cache->node.mm);
		}
	}

	cache->vaddr = 0;
	cache->page = -1;
}