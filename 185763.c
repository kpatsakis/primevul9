i915_gem_context_create_kernel(struct drm_i915_private *i915, int prio)
{
	struct i915_gem_context *ctx;
	int err;

	ctx = i915_gem_create_context(i915);
	if (IS_ERR(ctx))
		return ctx;

	err = i915_gem_context_pin_hw_id(ctx);
	if (err) {
		destroy_kernel_context(&ctx);
		return ERR_PTR(err);
	}

	i915_gem_context_clear_bannable(ctx);
	ctx->sched.priority = I915_USER_PRIORITY(prio);
	ctx->ring_size = PAGE_SIZE;

	GEM_BUG_ON(!i915_gem_context_is_kernel(ctx));

	return ctx;
}