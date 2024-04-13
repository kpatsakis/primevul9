i915_gem_userptr_init__mmu_notifier(struct drm_i915_gem_object *obj,
				    unsigned flags)
{
	struct i915_mmu_notifier *mn;
	struct i915_mmu_object *mo;

	if (flags & I915_USERPTR_UNSYNCHRONIZED)
		return capable(CAP_SYS_ADMIN) ? 0 : -EPERM;

	if (WARN_ON(obj->userptr.mm == NULL))
		return -EINVAL;

	mn = i915_mmu_notifier_find(obj->userptr.mm);
	if (IS_ERR(mn))
		return PTR_ERR(mn);

	mo = kzalloc(sizeof(*mo), GFP_KERNEL);
	if (!mo)
		return -ENOMEM;

	mo->mn = mn;
	mo->obj = obj;
	mo->it.start = obj->userptr.ptr;
	mo->it.last = obj->userptr.ptr + obj->base.size - 1;
	RB_CLEAR_NODE(&mo->it.rb);

	obj->userptr.mmu_object = mo;
	return 0;
}