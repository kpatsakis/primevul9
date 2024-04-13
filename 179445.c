static void eb_release_vmas(const struct i915_execbuffer *eb)
{
	const unsigned int count = eb->buffer_count;
	unsigned int i;

	for (i = 0; i < count; i++) {
		struct i915_vma *vma = eb->vma[i];
		unsigned int flags = eb->flags[i];

		if (!vma)
			break;

		GEM_BUG_ON(vma->exec_flags != &eb->flags[i]);
		vma->exec_flags = NULL;
		eb->vma[i] = NULL;

		if (flags & __EXEC_OBJECT_HAS_PIN)
			__eb_unreserve_vma(vma, flags);

		if (flags & __EXEC_OBJECT_HAS_REF)
			i915_vma_put(vma);
	}
}