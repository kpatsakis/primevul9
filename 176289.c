i915_mmu_notifier_find(struct i915_mm_struct *mm)
{
	struct i915_mmu_notifier *mn;
	int err = 0;

	mn = mm->mn;
	if (mn)
		return mn;

	mn = i915_mmu_notifier_create(mm);
	if (IS_ERR(mn))
		err = PTR_ERR(mn);

	down_write(&mm->mm->mmap_sem);
	mutex_lock(&mm->i915->mm_lock);
	if (mm->mn == NULL && !err) {
		/* Protected by mmap_sem (write-lock) */
		err = __mmu_notifier_register(&mn->mn, mm->mm);
		if (!err) {
			/* Protected by mm_lock */
			mm->mn = fetch_and_zero(&mn);
		}
	} else if (mm->mn) {
		/*
		 * Someone else raced and successfully installed the mmu
		 * notifier, we can cancel our own errors.
		 */
		err = 0;
	}
	mutex_unlock(&mm->i915->mm_lock);
	up_write(&mm->mm->mmap_sem);

	if (mn && !IS_ERR(mn))
		kfree(mn);

	return err ? ERR_PTR(err) : mm->mn;
}