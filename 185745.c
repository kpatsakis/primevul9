int i915_gem_contexts_init(struct drm_i915_private *dev_priv)
{
	struct i915_gem_context *ctx;

	/* Reassure ourselves we are only called once */
	GEM_BUG_ON(dev_priv->kernel_context);
	GEM_BUG_ON(dev_priv->preempt_context);

	intel_engine_init_ctx_wa(dev_priv->engine[RCS0]);
	init_contexts(dev_priv);

	/* lowest priority; idle task */
	ctx = i915_gem_context_create_kernel(dev_priv, I915_PRIORITY_MIN);
	if (IS_ERR(ctx)) {
		DRM_ERROR("Failed to create default global context\n");
		return PTR_ERR(ctx);
	}
	/*
	 * For easy recognisablity, we want the kernel context to be 0 and then
	 * all user contexts will have non-zero hw_id. Kernel contexts are
	 * permanently pinned, so that we never suffer a stall and can
	 * use them from any allocation context (e.g. for evicting other
	 * contexts and from inside the shrinker).
	 */
	GEM_BUG_ON(ctx->hw_id);
	GEM_BUG_ON(!atomic_read(&ctx->hw_id_pin_count));
	dev_priv->kernel_context = ctx;

	/* highest priority; preempting task */
	if (needs_preempt_context(dev_priv)) {
		ctx = i915_gem_context_create_kernel(dev_priv, INT_MAX);
		if (!IS_ERR(ctx))
			dev_priv->preempt_context = ctx;
		else
			DRM_ERROR("Failed to create preempt context; disabling preemption\n");
	}

	DRM_DEBUG_DRIVER("%s context support initialized\n",
			 DRIVER_CAPS(dev_priv)->has_logical_contexts ?
			 "logical" : "fake");
	return 0;
}