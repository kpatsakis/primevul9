void i915_lut_handle_free(struct i915_lut_handle *lut)
{
	return kmem_cache_free(global.slab_luts, lut);
}