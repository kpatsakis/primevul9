static inline bool eb_use_cmdparser(const struct i915_execbuffer *eb)
{
	return intel_engine_needs_cmd_parser(eb->engine) && eb->batch_len;
}