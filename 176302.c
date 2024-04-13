__i915_mm_struct_free(struct kref *kref)
{
	struct i915_mm_struct *mm = container_of(kref, typeof(*mm), kref);

	/* Protected by dev_priv->mm_lock */
	hash_del(&mm->node);
	mutex_unlock(&mm->i915->mm_lock);

	INIT_WORK(&mm->work, __i915_mm_struct_free__worker);
	queue_work(mm->i915->mm.userptr_wq, &mm->work);
}