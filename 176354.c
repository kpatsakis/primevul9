userptr_mn_invalidate_range_start(struct mmu_notifier *_mn,
				  const struct mmu_notifier_range *range)
{
	struct i915_mmu_notifier *mn =
		container_of(_mn, struct i915_mmu_notifier, mn);
	struct interval_tree_node *it;
	unsigned long end;
	int ret = 0;

	if (RB_EMPTY_ROOT(&mn->objects.rb_root))
		return 0;

	/* interval ranges are inclusive, but invalidate range is exclusive */
	end = range->end - 1;

	spin_lock(&mn->lock);
	it = interval_tree_iter_first(&mn->objects, range->start, end);
	while (it) {
		struct drm_i915_gem_object *obj;

		if (!mmu_notifier_range_blockable(range)) {
			ret = -EAGAIN;
			break;
		}

		/*
		 * The mmu_object is released late when destroying the
		 * GEM object so it is entirely possible to gain a
		 * reference on an object in the process of being freed
		 * since our serialisation is via the spinlock and not
		 * the struct_mutex - and consequently use it after it
		 * is freed and then double free it. To prevent that
		 * use-after-free we only acquire a reference on the
		 * object if it is not in the process of being destroyed.
		 */
		obj = container_of(it, struct i915_mmu_object, it)->obj;
		if (!kref_get_unless_zero(&obj->base.refcount)) {
			it = interval_tree_iter_next(it, range->start, end);
			continue;
		}
		spin_unlock(&mn->lock);

		ret = i915_gem_object_unbind(obj,
					     I915_GEM_OBJECT_UNBIND_ACTIVE |
					     I915_GEM_OBJECT_UNBIND_BARRIER);
		if (ret == 0)
			ret = __i915_gem_object_put_pages(obj);
		i915_gem_object_put(obj);
		if (ret)
			return ret;

		spin_lock(&mn->lock);

		/*
		 * As we do not (yet) protect the mmu from concurrent insertion
		 * over this range, there is no guarantee that this search will
		 * terminate given a pathologic workload.
		 */
		it = interval_tree_iter_first(&mn->objects, range->start, end);
	}
	spin_unlock(&mn->lock);

	return ret;

}