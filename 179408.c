static void *reloc_kmap(struct drm_i915_gem_object *obj,
			struct reloc_cache *cache,
			unsigned long page)
{
	void *vaddr;

	if (cache->vaddr) {
		kunmap_atomic(unmask_page(cache->vaddr));
	} else {
		unsigned int flushes;
		int err;

		err = i915_gem_obj_prepare_shmem_write(obj, &flushes);
		if (err)
			return ERR_PTR(err);

		BUILD_BUG_ON(KMAP & CLFLUSH_FLAGS);
		BUILD_BUG_ON((KMAP | CLFLUSH_FLAGS) & PAGE_MASK);

		cache->vaddr = flushes | KMAP;
		cache->node.mm = (void *)obj;
		if (flushes)
			mb();
	}

	vaddr = kmap_atomic(i915_gem_object_get_dirty_page(obj, page));
	cache->vaddr = unmask_flags(cache->vaddr) | (unsigned long)vaddr;
	cache->page = page;

	return vaddr;
}