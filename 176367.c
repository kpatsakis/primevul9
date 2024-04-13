i915_gem_userptr_init__mm_struct(struct drm_i915_gem_object *obj)
{
	struct drm_i915_private *dev_priv = to_i915(obj->base.dev);
	struct i915_mm_struct *mm;
	int ret = 0;

	/* During release of the GEM object we hold the struct_mutex. This
	 * precludes us from calling mmput() at that time as that may be
	 * the last reference and so call exit_mmap(). exit_mmap() will
	 * attempt to reap the vma, and if we were holding a GTT mmap
	 * would then call drm_gem_vm_close() and attempt to reacquire
	 * the struct mutex. So in order to avoid that recursion, we have
	 * to defer releasing the mm reference until after we drop the
	 * struct_mutex, i.e. we need to schedule a worker to do the clean
	 * up.
	 */
	mutex_lock(&dev_priv->mm_lock);
	mm = __i915_mm_struct_find(dev_priv, current->mm);
	if (mm == NULL) {
		mm = kmalloc(sizeof(*mm), GFP_KERNEL);
		if (mm == NULL) {
			ret = -ENOMEM;
			goto out;
		}

		kref_init(&mm->kref);
		mm->i915 = to_i915(obj->base.dev);

		mm->mm = current->mm;
		mmgrab(current->mm);

		mm->mn = NULL;

		/* Protected by dev_priv->mm_lock */
		hash_add(dev_priv->mm_structs,
			 &mm->node, (unsigned long)mm->mm);
	} else
		kref_get(&mm->kref);

	obj->userptr.mm = mm;
out:
	mutex_unlock(&dev_priv->mm_lock);
	return ret;
}