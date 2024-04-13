static int eb_prefault_relocations(const struct i915_execbuffer *eb)
{
	const unsigned int count = eb->buffer_count;
	unsigned int i;

	if (unlikely(i915_modparams.prefault_disable))
		return 0;

	for (i = 0; i < count; i++) {
		int err;

		err = check_relocations(&eb->exec[i]);
		if (err)
			return err;
	}

	return 0;
}