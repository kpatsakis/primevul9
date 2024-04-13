eb_relocate_vma_slow(struct i915_execbuffer *eb, struct i915_vma *vma)
{
	const struct drm_i915_gem_exec_object2 *entry = exec_entry(eb, vma);
	struct drm_i915_gem_relocation_entry *relocs =
		u64_to_ptr(typeof(*relocs), entry->relocs_ptr);
	unsigned int i;
	int err;

	for (i = 0; i < entry->relocation_count; i++) {
		u64 offset = eb_relocate_entry(eb, vma, &relocs[i]);

		if ((s64)offset < 0) {
			err = (int)offset;
			goto err;
		}
	}
	err = 0;
err:
	reloc_cache_reset(&eb->reloc_cache);
	return err;
}