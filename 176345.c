i915_mmu_notifier_free(struct i915_mmu_notifier *mn,
		       struct mm_struct *mm)
{
	if (mn == NULL)
		return;

	mmu_notifier_unregister(&mn->mn, mm);
	kfree(mn);
}