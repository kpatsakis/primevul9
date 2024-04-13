eb_vma_misplaced(const struct drm_i915_gem_exec_object2 *entry,
		 const struct i915_vma *vma,
		 unsigned int flags)
{
	if (vma->node.size < entry->pad_to_size)
		return true;

	if (entry->alignment && !IS_ALIGNED(vma->node.start, entry->alignment))
		return true;

	if (flags & EXEC_OBJECT_PINNED &&
	    vma->node.start != entry->offset)
		return true;

	if (flags & __EXEC_OBJECT_NEEDS_BIAS &&
	    vma->node.start < BATCH_OFFSET_BIAS)
		return true;

	if (!(flags & EXEC_OBJECT_SUPPORTS_48B_ADDRESS) &&
	    (vma->node.start + vma->node.size - 1) >> 32)
		return true;

	if (flags & __EXEC_OBJECT_NEEDS_MAP &&
	    !i915_vma_is_map_and_fenceable(vma))
		return true;

	return false;
}