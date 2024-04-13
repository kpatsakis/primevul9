__free_fence_array(struct drm_syncobj **fences, unsigned int n)
{
	while (n--)
		drm_syncobj_put(ptr_mask_bits(fences[n], 2));
	kvfree(fences);
}