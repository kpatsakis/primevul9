eb_create(struct drm_i915_gem_execbuffer2 *args)
{
	struct eb_objects *eb = NULL;

	if (args->flags & I915_EXEC_HANDLE_LUT) {
		int size = args->buffer_count;
		size *= sizeof(struct drm_i915_gem_object *);
		size += sizeof(struct eb_objects);
		eb = kmalloc(size, GFP_TEMPORARY | __GFP_NOWARN | __GFP_NORETRY);
	}

	if (eb == NULL) {
		int size = args->buffer_count;
		int count = PAGE_SIZE / sizeof(struct hlist_head) / 2;
		BUILD_BUG_ON(!is_power_of_2(PAGE_SIZE / sizeof(struct hlist_head)));
		while (count > 2*size)
			count >>= 1;
		eb = kzalloc(count*sizeof(struct hlist_head) +
			     sizeof(struct eb_objects),
			     GFP_TEMPORARY);
		if (eb == NULL)
			return eb;

		eb->and = count - 1;
	} else
		eb->and = -args->buffer_count;

	INIT_LIST_HEAD(&eb->objects);
	return eb;
}