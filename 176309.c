i915_gem_userptr_release__mmu_notifier(struct drm_i915_gem_object *obj)
{
	struct i915_mmu_object *mo;

	mo = fetch_and_zero(&obj->userptr.mmu_object);
	if (!mo)
		return;

	spin_lock(&mo->mn->lock);
	del_object(mo);
	spin_unlock(&mo->mn->lock);
	kfree(mo);
}