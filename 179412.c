static void *reloc_vaddr(struct drm_i915_gem_object *obj,
			 struct reloc_cache *cache,
			 unsigned long page)
{
	void *vaddr;

	if (cache->page == page) {
		vaddr = unmask_page(cache->vaddr);
	} else {
		vaddr = NULL;
		if ((cache->vaddr & KMAP) == 0)
			vaddr = reloc_iomap(obj, cache, page);
		if (!vaddr)
			vaddr = reloc_kmap(obj, cache, page);
	}

	return vaddr;
}