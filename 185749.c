void i915_gem_context_release(struct kref *ref)
{
	struct i915_gem_context *ctx = container_of(ref, typeof(*ctx), ref);
	struct drm_i915_private *i915 = ctx->i915;

	trace_i915_context_free(ctx);
	if (llist_add(&ctx->free_link, &i915->contexts.free_list))
		queue_work(i915->wq, &i915->contexts.free_work);
}