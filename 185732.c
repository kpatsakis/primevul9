static void contexts_free(struct drm_i915_private *i915)
{
	struct llist_node *freed = llist_del_all(&i915->contexts.free_list);
	struct i915_gem_context *ctx, *cn;

	lockdep_assert_held(&i915->drm.struct_mutex);

	llist_for_each_entry_safe(ctx, cn, freed, free_link)
		i915_gem_context_free(ctx);
}