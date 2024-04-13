void i915_gem_contexts_fini(struct drm_i915_private *i915)
{
	lockdep_assert_held(&i915->drm.struct_mutex);

	if (i915->preempt_context)
		destroy_kernel_context(&i915->preempt_context);
	destroy_kernel_context(&i915->kernel_context);

	/* Must free all deferred contexts (via flush_workqueue) first */
	GEM_BUG_ON(!list_empty(&i915->contexts.hw_id_list));
	ida_destroy(&i915->contexts.hw_ida);
}