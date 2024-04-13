eb_lookup_objects(struct eb_objects *eb,
		  struct drm_i915_gem_exec_object2 *exec,
		  const struct drm_i915_gem_execbuffer2 *args,
		  struct drm_file *file)
{
	int i;

	spin_lock(&file->table_lock);
	for (i = 0; i < args->buffer_count; i++) {
		struct drm_i915_gem_object *obj;

		obj = to_intel_bo(idr_find(&file->object_idr, exec[i].handle));
		if (obj == NULL) {
			spin_unlock(&file->table_lock);
			DRM_DEBUG("Invalid object handle %d at index %d\n",
				   exec[i].handle, i);
			return -ENOENT;
		}

		if (!list_empty(&obj->exec_list)) {
			spin_unlock(&file->table_lock);
			DRM_DEBUG("Object %p [handle %d, index %d] appears more than once in object list\n",
				   obj, exec[i].handle, i);
			return -EINVAL;
		}

		drm_gem_object_reference(&obj->base);
		list_add_tail(&obj->exec_list, &eb->objects);

		obj->exec_entry = &exec[i];
		if (eb->and < 0) {
			eb->lut[i] = obj;
		} else {
			uint32_t handle = args->flags & I915_EXEC_HANDLE_LUT ? i : exec[i].handle;
			obj->exec_handle = handle;
			hlist_add_head(&obj->exec_node,
				       &eb->buckets[handle & eb->and]);
		}
	}
	spin_unlock(&file->table_lock);

	return 0;
}