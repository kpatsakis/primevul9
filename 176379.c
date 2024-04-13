i915_gem_userptr_dmabuf_export(struct drm_i915_gem_object *obj)
{
	if (obj->userptr.mmu_object)
		return 0;

	return i915_gem_userptr_init__mmu_notifier(obj, 0);
}