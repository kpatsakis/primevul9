__create_context(struct drm_i915_private *dev_priv)
{
	struct i915_gem_context *ctx;
	int i;

	ctx = kzalloc(sizeof(*ctx), GFP_KERNEL);
	if (!ctx)
		return ERR_PTR(-ENOMEM);

	kref_init(&ctx->ref);
	list_add_tail(&ctx->link, &dev_priv->contexts.list);
	ctx->i915 = dev_priv;
	ctx->sched.priority = I915_USER_PRIORITY(I915_PRIORITY_NORMAL);
	INIT_LIST_HEAD(&ctx->active_engines);
	mutex_init(&ctx->mutex);

	ctx->hw_contexts = RB_ROOT;
	spin_lock_init(&ctx->hw_contexts_lock);

	INIT_RADIX_TREE(&ctx->handles_vma, GFP_KERNEL);
	INIT_LIST_HEAD(&ctx->handles_list);
	INIT_LIST_HEAD(&ctx->hw_id_link);

	/* NB: Mark all slices as needing a remap so that when the context first
	 * loads it will restore whatever remap state already exists. If there
	 * is no remap info, it will be a NOP. */
	ctx->remap_slice = ALL_L3_SLICES(dev_priv);

	i915_gem_context_set_bannable(ctx);
	i915_gem_context_set_recoverable(ctx);

	ctx->ring_size = 4 * PAGE_SIZE;
	ctx->desc_template =
		default_desc_template(dev_priv, dev_priv->mm.aliasing_ppgtt);

	for (i = 0; i < ARRAY_SIZE(ctx->hang_timestamp); i++)
		ctx->hang_timestamp[i] = jiffies - CONTEXT_FAST_HANG_JIFFIES;

	return ctx;
}