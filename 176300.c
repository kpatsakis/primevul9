__i915_mm_struct_find(struct drm_i915_private *dev_priv, struct mm_struct *real)
{
	struct i915_mm_struct *mm;

	/* Protected by dev_priv->mm_lock */
	hash_for_each_possible(dev_priv->mm_structs, mm, node, (unsigned long)real)
		if (mm->mm == real)
			return mm;

	return NULL;
}