static void release_hw_id(struct i915_gem_context *ctx)
{
	struct drm_i915_private *i915 = ctx->i915;

	if (list_empty(&ctx->hw_id_link))
		return;

	mutex_lock(&i915->contexts.mutex);
	if (!list_empty(&ctx->hw_id_link)) {
		ida_simple_remove(&i915->contexts.hw_ida, ctx->hw_id);
		list_del_init(&ctx->hw_id_link);
	}
	mutex_unlock(&i915->contexts.mutex);
}