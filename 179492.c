static size_t eb_element_size(void)
{
	return (sizeof(struct drm_i915_gem_exec_object2) +
		sizeof(struct i915_vma *) +
		sizeof(unsigned int));
}