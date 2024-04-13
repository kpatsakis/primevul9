i915_gem_userptr_release(struct drm_i915_gem_object *obj)
{
	i915_gem_userptr_release__mmu_notifier(obj);
	i915_gem_userptr_release__mm_struct(obj);
}