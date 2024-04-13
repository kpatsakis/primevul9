static unsigned int eb_batch_index(const struct i915_execbuffer *eb)
{
	if (eb->args->flags & I915_EXEC_BATCH_FIRST)
		return 0;
	else
		return eb->buffer_count - 1;
}