static void lut_close(struct i915_gem_context *ctx)
{
	struct i915_lut_handle *lut, *ln;
	struct radix_tree_iter iter;
	void __rcu **slot;

	list_for_each_entry_safe(lut, ln, &ctx->handles_list, ctx_link) {
		list_del(&lut->obj_link);
		i915_lut_handle_free(lut);
	}

	rcu_read_lock();
	radix_tree_for_each_slot(slot, &ctx->handles_vma, &iter, 0) {
		struct i915_vma *vma = rcu_dereference_raw(*slot);

		radix_tree_iter_delete(&ctx->handles_vma, &iter, slot);
		__i915_gem_object_release_unless_active(vma->obj);
	}
	rcu_read_unlock();
}