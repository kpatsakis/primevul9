struct i915_lut_handle *i915_lut_handle_alloc(void)
{
	return kmem_cache_alloc(global.slab_luts, GFP_KERNEL);
}