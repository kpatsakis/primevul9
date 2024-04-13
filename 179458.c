static void eb_destroy(const struct i915_execbuffer *eb)
{
	GEM_BUG_ON(eb->reloc_cache.rq);

	if (eb->lut_size > 0)
		kfree(eb->buckets);
}