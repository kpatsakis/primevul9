static void contexts_free_first(struct drm_i915_private *i915)
{
	struct i915_gem_context *ctx;
	struct llist_node *freed;

	lockdep_assert_held(&i915->drm.struct_mutex);

	freed = llist_del_first(&i915->contexts.free_list);
	if (!freed)
		return;

	ctx = container_of(freed, typeof(*ctx), free_link);
	i915_gem_context_free(ctx);
}