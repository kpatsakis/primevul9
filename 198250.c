i915_gem_execbuffer_retire_commands(struct drm_device *dev,
				    struct drm_file *file,
				    struct intel_ring_buffer *ring)
{
	/* Unconditionally force add_request to emit a full flush. */
	ring->gpu_caches_dirty = true;

	/* Add a breadcrumb for the completion of the batch buffer */
	(void)i915_add_request(ring, file, NULL);
}