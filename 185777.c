static void init_contexts(struct drm_i915_private *i915)
{
	mutex_init(&i915->contexts.mutex);
	INIT_LIST_HEAD(&i915->contexts.list);

	/* Using the simple ida interface, the max is limited by sizeof(int) */
	BUILD_BUG_ON(MAX_CONTEXT_HW_ID > INT_MAX);
	BUILD_BUG_ON(GEN11_MAX_CONTEXT_HW_ID > INT_MAX);
	ida_init(&i915->contexts.hw_ida);
	INIT_LIST_HEAD(&i915->contexts.hw_id_list);

	INIT_WORK(&i915->contexts.free_work, contexts_free_worker);
	init_llist_head(&i915->contexts.free_list);
}