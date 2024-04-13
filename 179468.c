put_fence_array(struct drm_i915_gem_execbuffer2 *args,
		struct drm_syncobj **fences)
{
	if (fences)
		__free_fence_array(fences, args->num_cliprects);
}