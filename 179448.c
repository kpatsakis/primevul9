await_fence_array(struct i915_execbuffer *eb,
		  struct drm_syncobj **fences)
{
	const unsigned int nfences = eb->args->num_cliprects;
	unsigned int n;
	int err;

	for (n = 0; n < nfences; n++) {
		struct drm_syncobj *syncobj;
		struct dma_fence *fence;
		unsigned int flags;

		syncobj = ptr_unpack_bits(fences[n], &flags, 2);
		if (!(flags & I915_EXEC_FENCE_WAIT))
			continue;

		fence = drm_syncobj_fence_get(syncobj);
		if (!fence)
			return -EINVAL;

		err = i915_request_await_dma_fence(eb->request, fence);
		dma_fence_put(fence);
		if (err < 0)
			return err;
	}

	return 0;
}