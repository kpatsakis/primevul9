i915_gem_execbuffer_reserve(struct intel_ring_buffer *ring,
			    struct drm_file *file,
			    struct list_head *objects,
			    bool *need_relocs)
{
	struct drm_i915_gem_object *obj;
	struct list_head ordered_objects;
	bool has_fenced_gpu_access = INTEL_INFO(ring->dev)->gen < 4;
	int retry;

	INIT_LIST_HEAD(&ordered_objects);
	while (!list_empty(objects)) {
		struct drm_i915_gem_exec_object2 *entry;
		bool need_fence, need_mappable;

		obj = list_first_entry(objects,
				       struct drm_i915_gem_object,
				       exec_list);
		entry = obj->exec_entry;

		need_fence =
			has_fenced_gpu_access &&
			entry->flags & EXEC_OBJECT_NEEDS_FENCE &&
			obj->tiling_mode != I915_TILING_NONE;
		need_mappable = need_fence || need_reloc_mappable(obj);

		if (need_mappable)
			list_move(&obj->exec_list, &ordered_objects);
		else
			list_move_tail(&obj->exec_list, &ordered_objects);

		obj->base.pending_read_domains = I915_GEM_GPU_DOMAINS & ~I915_GEM_DOMAIN_COMMAND;
		obj->base.pending_write_domain = 0;
		obj->pending_fenced_gpu_access = false;
	}
	list_splice(&ordered_objects, objects);

	/* Attempt to pin all of the buffers into the GTT.
	 * This is done in 3 phases:
	 *
	 * 1a. Unbind all objects that do not match the GTT constraints for
	 *     the execbuffer (fenceable, mappable, alignment etc).
	 * 1b. Increment pin count for already bound objects.
	 * 2.  Bind new objects.
	 * 3.  Decrement pin count.
	 *
	 * This avoid unnecessary unbinding of later objects in order to make
	 * room for the earlier objects *unless* we need to defragment.
	 */
	retry = 0;
	do {
		int ret = 0;

		/* Unbind any ill-fitting objects or pin. */
		list_for_each_entry(obj, objects, exec_list) {
			struct drm_i915_gem_exec_object2 *entry = obj->exec_entry;
			bool need_fence, need_mappable;

			if (!obj->gtt_space)
				continue;

			need_fence =
				has_fenced_gpu_access &&
				entry->flags & EXEC_OBJECT_NEEDS_FENCE &&
				obj->tiling_mode != I915_TILING_NONE;
			need_mappable = need_fence || need_reloc_mappable(obj);

			if ((entry->alignment && obj->gtt_offset & (entry->alignment - 1)) ||
			    (need_mappable && !obj->map_and_fenceable))
				ret = i915_gem_object_unbind(obj);
			else
				ret = i915_gem_execbuffer_reserve_object(obj, ring, need_relocs);
			if (ret)
				goto err;
		}

		/* Bind fresh objects */
		list_for_each_entry(obj, objects, exec_list) {
			if (obj->gtt_space)
				continue;

			ret = i915_gem_execbuffer_reserve_object(obj, ring, need_relocs);
			if (ret)
				goto err;
		}

err:		/* Decrement pin count for bound objects */
		list_for_each_entry(obj, objects, exec_list)
			i915_gem_execbuffer_unreserve_object(obj);

		if (ret != -ENOSPC || retry++)
			return ret;

		ret = i915_gem_evict_everything(ring->dev);
		if (ret)
			return ret;
	} while (1);
}