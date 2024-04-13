eb_unreserve_vma(struct i915_vma *vma, unsigned int *flags)
{
	if (!(*flags & __EXEC_OBJECT_HAS_PIN))
		return;

	__eb_unreserve_vma(vma, *flags);
	*flags &= ~__EXEC_OBJECT_RESERVED;
}