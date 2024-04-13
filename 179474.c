static void eb_reset_vmas(const struct i915_execbuffer *eb)
{
	eb_release_vmas(eb);
	if (eb->lut_size > 0)
		memset(eb->buckets, 0,
		       sizeof(struct hlist_head) << eb->lut_size);
}