static void i915_global_gem_context_shrink(void)
{
	kmem_cache_shrink(global.slab_luts);
}