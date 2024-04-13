static void i915_gem_context_free(struct i915_gem_context *ctx)
{
	struct intel_context *it, *n;

	lockdep_assert_held(&ctx->i915->drm.struct_mutex);
	GEM_BUG_ON(!i915_gem_context_is_closed(ctx));
	GEM_BUG_ON(!list_empty(&ctx->active_engines));

	release_hw_id(ctx);
	i915_ppgtt_put(ctx->ppgtt);

	rbtree_postorder_for_each_entry_safe(it, n, &ctx->hw_contexts, node)
		intel_context_put(it);

	kfree(ctx->name);
	put_pid(ctx->pid);

	list_del(&ctx->link);
	mutex_destroy(&ctx->mutex);

	kfree_rcu(ctx, rcu);
}