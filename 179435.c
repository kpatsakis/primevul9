static inline int use_cpu_reloc(const struct reloc_cache *cache,
				const struct drm_i915_gem_object *obj)
{
	if (!i915_gem_object_has_struct_page(obj))
		return false;

	if (DBG_FORCE_RELOC == FORCE_CPU_RELOC)
		return true;

	if (DBG_FORCE_RELOC == FORCE_GTT_RELOC)
		return false;

	return (cache->has_llc ||
		obj->cache_dirty ||
		obj->cache_level != I915_CACHE_NONE);
}