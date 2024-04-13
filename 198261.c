eb_get_object(struct eb_objects *eb, unsigned long handle)
{
	if (eb->and < 0) {
		if (handle >= -eb->and)
			return NULL;
		return eb->lut[handle];
	} else {
		struct hlist_head *head;
		struct hlist_node *node;

		head = &eb->buckets[handle & eb->and];
		hlist_for_each(node, head) {
			struct drm_i915_gem_object *obj;

			obj = hlist_entry(node, struct drm_i915_gem_object, exec_node);
			if (obj->exec_handle == handle)
				return obj;
		}
		return NULL;
	}
}