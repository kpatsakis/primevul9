static int eb_relocate(struct i915_execbuffer *eb)
{
	if (eb_lookup_vmas(eb))
		goto slow;

	/* The objects are in their final locations, apply the relocations. */
	if (eb->args->flags & __EXEC_HAS_RELOC) {
		struct i915_vma *vma;

		list_for_each_entry(vma, &eb->relocs, reloc_link) {
			if (eb_relocate_vma(eb, vma))
				goto slow;
		}
	}

	return 0;

slow:
	return eb_relocate_slow(eb);
}