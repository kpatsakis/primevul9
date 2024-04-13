i915_gem_execbuffer_reserve_object(struct drm_i915_gem_object *obj,
				   struct intel_ring_buffer *ring,
				   bool *need_reloc)
{
	struct drm_i915_private *dev_priv = obj->base.dev->dev_private;
	struct drm_i915_gem_exec_object2 *entry = obj->exec_entry;
	bool has_fenced_gpu_access = INTEL_INFO(ring->dev)->gen < 4;
	bool need_fence, need_mappable;
	int ret;

	need_fence =
		has_fenced_gpu_access &&
		entry->flags & EXEC_OBJECT_NEEDS_FENCE &&
		obj->tiling_mode != I915_TILING_NONE;
	need_mappable = need_fence || need_reloc_mappable(obj);

	ret = i915_gem_object_pin(obj, entry->alignment, need_mappable, false);
	if (ret)
		return ret;

	entry->flags |= __EXEC_OBJECT_HAS_PIN;

	if (has_fenced_gpu_access) {
		if (entry->flags & EXEC_OBJECT_NEEDS_FENCE) {
			ret = i915_gem_object_get_fence(obj);
			if (ret)
				return ret;

			if (i915_gem_object_pin_fence(obj))
				entry->flags |= __EXEC_OBJECT_HAS_FENCE;

			obj->pending_fenced_gpu_access = true;
		}
	}

	/* Ensure ppgtt mapping exists if needed */
	if (dev_priv->mm.aliasing_ppgtt && !obj->has_aliasing_ppgtt_mapping) {
		i915_ppgtt_bind_object(dev_priv->mm.aliasing_ppgtt,
				       obj, obj->cache_level);

		obj->has_aliasing_ppgtt_mapping = 1;
	}

	if (entry->offset != obj->gtt_offset) {
		entry->offset = obj->gtt_offset;
		*need_reloc = true;
	}

	if (entry->flags & EXEC_OBJECT_WRITE) {
		obj->base.pending_read_domains = I915_GEM_DOMAIN_RENDER;
		obj->base.pending_write_domain = I915_GEM_DOMAIN_RENDER;
	}

	if (entry->flags & EXEC_OBJECT_NEEDS_GTT &&
	    !obj->has_global_gtt_mapping)
		i915_gem_gtt_bind_object(obj, obj->cache_level);

	return 0;
}