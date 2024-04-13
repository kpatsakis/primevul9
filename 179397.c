static inline void __eb_unreserve_vma(struct i915_vma *vma, unsigned int flags)
{
	GEM_BUG_ON(!(flags & __EXEC_OBJECT_HAS_PIN));

	if (unlikely(flags & __EXEC_OBJECT_HAS_FENCE))
		__i915_vma_unpin_fence(vma);

	__i915_vma_unpin(vma);
}