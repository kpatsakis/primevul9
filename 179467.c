static inline struct i915_ggtt *cache_to_ggtt(struct reloc_cache *cache)
{
	struct drm_i915_private *i915 =
		container_of(cache, struct i915_execbuffer, reloc_cache)->i915;
	return &i915->ggtt;
}