static int eb_reserve_vma(const struct i915_execbuffer *eb,
			  struct i915_vma *vma)
{
	struct drm_i915_gem_exec_object2 *entry = exec_entry(eb, vma);
	unsigned int exec_flags = *vma->exec_flags;
	u64 pin_flags;
	int err;

	pin_flags = PIN_USER | PIN_NONBLOCK;
	if (exec_flags & EXEC_OBJECT_NEEDS_GTT)
		pin_flags |= PIN_GLOBAL;

	/*
	 * Wa32bitGeneralStateOffset & Wa32bitInstructionBaseOffset,
	 * limit address to the first 4GBs for unflagged objects.
	 */
	if (!(exec_flags & EXEC_OBJECT_SUPPORTS_48B_ADDRESS))
		pin_flags |= PIN_ZONE_4G;

	if (exec_flags & __EXEC_OBJECT_NEEDS_MAP)
		pin_flags |= PIN_MAPPABLE;

	if (exec_flags & EXEC_OBJECT_PINNED) {
		pin_flags |= entry->offset | PIN_OFFSET_FIXED;
		pin_flags &= ~PIN_NONBLOCK; /* force overlapping checks */
	} else if (exec_flags & __EXEC_OBJECT_NEEDS_BIAS) {
		pin_flags |= BATCH_OFFSET_BIAS | PIN_OFFSET_BIAS;
	}

	err = i915_vma_pin(vma,
			   entry->pad_to_size, entry->alignment,
			   pin_flags);
	if (err)
		return err;

	if (entry->offset != vma->node.start) {
		entry->offset = vma->node.start | UPDATE;
		eb->args->flags |= __EXEC_HAS_RELOC;
	}

	if (unlikely(exec_flags & EXEC_OBJECT_NEEDS_FENCE)) {
		err = i915_vma_pin_fence(vma);
		if (unlikely(err)) {
			i915_vma_unpin(vma);
			return err;
		}

		if (vma->fence)
			exec_flags |= __EXEC_OBJECT_HAS_FENCE;
	}

	*vma->exec_flags = exec_flags | __EXEC_OBJECT_HAS_PIN;
	GEM_BUG_ON(eb_vma_misplaced(entry, vma, exec_flags));

	return 0;
}