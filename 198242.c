i915_gem_execbuffer_move_to_gpu(struct intel_ring_buffer *ring,
				struct list_head *objects)
{
	struct drm_i915_gem_object *obj;
	uint32_t flush_domains = 0;
	int ret;

	list_for_each_entry(obj, objects, exec_list) {
		ret = i915_gem_object_sync(obj, ring);
		if (ret)
			return ret;

		if (obj->base.write_domain & I915_GEM_DOMAIN_CPU)
			i915_gem_clflush_object(obj);

		flush_domains |= obj->base.write_domain;
	}

	if (flush_domains & I915_GEM_DOMAIN_CPU)
		i915_gem_chipset_flush(ring->dev);

	if (flush_domains & I915_GEM_DOMAIN_GTT)
		wmb();

	/* Unconditionally invalidate gpu caches and ensure that we do flush
	 * any residual writes from the previous batch.
	 */
	return intel_ring_invalidate_all_caches(ring);
}