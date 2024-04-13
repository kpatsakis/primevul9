eb_select_engine(struct drm_i915_private *dev_priv,
		 struct drm_file *file,
		 struct drm_i915_gem_execbuffer2 *args)
{
	unsigned int user_ring_id = args->flags & I915_EXEC_RING_MASK;
	struct intel_engine_cs *engine;

	if (user_ring_id > I915_USER_RINGS) {
		DRM_DEBUG("execbuf with unknown ring: %u\n", user_ring_id);
		return NULL;
	}

	if ((user_ring_id != I915_EXEC_BSD) &&
	    ((args->flags & I915_EXEC_BSD_MASK) != 0)) {
		DRM_DEBUG("execbuf with non bsd ring but with invalid "
			  "bsd dispatch flags: %d\n", (int)(args->flags));
		return NULL;
	}

	if (user_ring_id == I915_EXEC_BSD && HAS_BSD2(dev_priv)) {
		unsigned int bsd_idx = args->flags & I915_EXEC_BSD_MASK;

		if (bsd_idx == I915_EXEC_BSD_DEFAULT) {
			bsd_idx = gen8_dispatch_bsd_engine(dev_priv, file);
		} else if (bsd_idx >= I915_EXEC_BSD_RING1 &&
			   bsd_idx <= I915_EXEC_BSD_RING2) {
			bsd_idx >>= I915_EXEC_BSD_SHIFT;
			bsd_idx--;
		} else {
			DRM_DEBUG("execbuf with unknown bsd ring: %u\n",
				  bsd_idx);
			return NULL;
		}

		engine = dev_priv->engine[_VCS(bsd_idx)];
	} else {
		engine = dev_priv->engine[user_ring_map[user_ring_id]];
	}

	if (!engine) {
		DRM_DEBUG("execbuf with invalid ring: %u\n", user_ring_id);
		return NULL;
	}

	return engine;
}