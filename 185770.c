int __init i915_global_gem_context_init(void)
{
	global.slab_luts = KMEM_CACHE(i915_lut_handle, 0);
	if (!global.slab_luts)
		return -ENOMEM;

	i915_global_register(&global.base);
	return 0;
}