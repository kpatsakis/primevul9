i915_gem_create_context(struct drm_i915_private *dev_priv)
{
	struct i915_gem_context *ctx;

	lockdep_assert_held(&dev_priv->drm.struct_mutex);

	/* Reap the most stale context */
	contexts_free_first(dev_priv);

	ctx = __create_context(dev_priv);
	if (IS_ERR(ctx))
		return ctx;

	if (HAS_FULL_PPGTT(dev_priv)) {
		struct i915_hw_ppgtt *ppgtt;

		ppgtt = i915_ppgtt_create(dev_priv);
		if (IS_ERR(ppgtt)) {
			DRM_DEBUG_DRIVER("PPGTT setup failed (%ld)\n",
					 PTR_ERR(ppgtt));
			context_close(ctx);
			return ERR_CAST(ppgtt);
		}

		ctx->ppgtt = ppgtt;
		ctx->desc_template = default_desc_template(dev_priv, ppgtt);
	}

	trace_i915_context_create(ctx);

	return ctx;
}