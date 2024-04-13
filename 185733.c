destroy_kernel_context(struct i915_gem_context **ctxp)
{
	struct i915_gem_context *ctx;

	/* Keep the context ref so that we can free it immediately ourselves */
	ctx = i915_gem_context_get(fetch_and_zero(ctxp));
	GEM_BUG_ON(!i915_gem_context_is_kernel(ctx));

	context_close(ctx);
	i915_gem_context_free(ctx);
}