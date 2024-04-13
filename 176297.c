__i915_gem_userptr_set_active(struct drm_i915_gem_object *obj, bool value)
{
	struct i915_mmu_object *mo = obj->userptr.mmu_object;

	/*
	 * During mm_invalidate_range we need to cancel any userptr that
	 * overlaps the range being invalidated. Doing so requires the
	 * struct_mutex, and that risks recursion. In order to cause
	 * recursion, the user must alias the userptr address space with
	 * a GTT mmapping (possible with a MAP_FIXED) - then when we have
	 * to invalidate that mmaping, mm_invalidate_range is called with
	 * the userptr address *and* the struct_mutex held.  To prevent that
	 * we set a flag under the i915_mmu_notifier spinlock to indicate
	 * whether this object is valid.
	 */
	if (!mo)
		return;

	spin_lock(&mo->mn->lock);
	if (value)
		add_object(mo);
	else
		del_object(mo);
	spin_unlock(&mo->mn->lock);
}