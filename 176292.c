i915_mmu_notifier_create(struct i915_mm_struct *mm)
{
	struct i915_mmu_notifier *mn;

	mn = kmalloc(sizeof(*mn), GFP_KERNEL);
	if (mn == NULL)
		return ERR_PTR(-ENOMEM);

	spin_lock_init(&mn->lock);
	mn->mn.ops = &i915_gem_userptr_notifier;
	mn->objects = RB_ROOT_CACHED;
	mn->mm = mm;

	return mn;
}