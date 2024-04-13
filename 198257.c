eb_destroy(struct eb_objects *eb)
{
	while (!list_empty(&eb->objects)) {
		struct drm_i915_gem_object *obj;

		obj = list_first_entry(&eb->objects,
				       struct drm_i915_gem_object,
				       exec_list);
		list_del_init(&obj->exec_list);
		drm_gem_object_unreference(&obj->base);
	}
	kfree(eb);
}