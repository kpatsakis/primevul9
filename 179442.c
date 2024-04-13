signal_fence_array(struct i915_execbuffer *eb,
		   struct drm_syncobj **fences)
{
	const unsigned int nfences = eb->args->num_cliprects;
	struct dma_fence * const fence = &eb->request->fence;
	unsigned int n;

	for (n = 0; n < nfences; n++) {
		struct drm_syncobj *syncobj;
		unsigned int flags;

		syncobj = ptr_unpack_bits(fences[n], &flags, 2);
		if (!(flags & I915_EXEC_FENCE_SIGNAL))
			continue;

		drm_syncobj_replace_fence(syncobj, fence);
	}
}