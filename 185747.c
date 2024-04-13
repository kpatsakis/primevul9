static void i915_global_gem_context_exit(void)
{
	kmem_cache_destroy(global.slab_luts);
}