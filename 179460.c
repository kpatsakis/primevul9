eb_pin_vma(struct i915_execbuffer *eb,
	   const struct drm_i915_gem_exec_object2 *entry,
	   struct i915_vma *vma)
{
	unsigned int exec_flags = *vma->exec_flags;
	u64 pin_flags;

	if (vma->node.size)
		pin_flags = vma->node.start;
	else
		pin_flags = entry->offset & PIN_OFFSET_MASK;

	pin_flags |= PIN_USER | PIN_NOEVICT | PIN_OFFSET_FIXED;
	if (unlikely(exec_flags & EXEC_OBJECT_NEEDS_GTT))
		pin_flags |= PIN_GLOBAL;

	if (unlikely(i915_vma_pin(vma, 0, 0, pin_flags)))
		return false;

	if (unlikely(exec_flags & EXEC_OBJECT_NEEDS_FENCE)) {
		if (unlikely(i915_vma_pin_fence(vma))) {
			i915_vma_unpin(vma);
			return false;
		}

		if (vma->fence)
			exec_flags |= __EXEC_OBJECT_HAS_FENCE;
	}

	*vma->exec_flags = exec_flags | __EXEC_OBJECT_HAS_PIN;
	return !eb_vma_misplaced(entry, vma, exec_flags);
}