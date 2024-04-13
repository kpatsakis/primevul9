i915_gem_context_reconfigure_sseu(struct i915_gem_context *ctx,
				  struct intel_engine_cs *engine,
				  struct intel_sseu sseu)
{
	int ret;

	ret = mutex_lock_interruptible(&ctx->i915->drm.struct_mutex);
	if (ret)
		return ret;

	ret = __i915_gem_context_reconfigure_sseu(ctx, engine, sseu);

	mutex_unlock(&ctx->i915->drm.struct_mutex);

	return ret;
}