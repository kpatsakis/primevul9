static void *reloc_iomap(struct drm_i915_gem_object *obj,
			 struct reloc_cache *cache,
			 unsigned long page)
{
	struct i915_ggtt *ggtt = cache_to_ggtt(cache);
	unsigned long offset;
	void *vaddr;

	if (cache->vaddr) {
		io_mapping_unmap_atomic((void __force __iomem *) unmask_page(cache->vaddr));
	} else {
		struct i915_vma *vma;
		int err;

		if (use_cpu_reloc(cache, obj))
			return NULL;

		err = i915_gem_object_set_to_gtt_domain(obj, true);
		if (err)
			return ERR_PTR(err);

		vma = i915_gem_object_ggtt_pin(obj, NULL, 0, 0,
					       PIN_MAPPABLE |
					       PIN_NONBLOCK |
					       PIN_NONFAULT);
		if (IS_ERR(vma)) {
			memset(&cache->node, 0, sizeof(cache->node));
			err = drm_mm_insert_node_in_range
				(&ggtt->vm.mm, &cache->node,
				 PAGE_SIZE, 0, I915_COLOR_UNEVICTABLE,
				 0, ggtt->mappable_end,
				 DRM_MM_INSERT_LOW);
			if (err) /* no inactive aperture space, use cpu reloc */
				return NULL;
		} else {
			err = i915_vma_put_fence(vma);
			if (err) {
				i915_vma_unpin(vma);
				return ERR_PTR(err);
			}

			cache->node.start = vma->node.start;
			cache->node.mm = (void *)vma;
		}
	}

	offset = cache->node.start;
	if (cache->node.allocated) {
		wmb();
		ggtt->vm.insert_page(&ggtt->vm,
				     i915_gem_object_get_dma_address(obj, page),
				     offset, I915_CACHE_NONE, 0);
	} else {
		offset += page << PAGE_SHIFT;
	}

	vaddr = (void __force *)io_mapping_map_atomic_wc(&ggtt->iomap,
							 offset);
	cache->page = page;
	cache->vaddr = (unsigned long)vaddr;

	return vaddr;
}