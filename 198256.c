i915_gem_execbuffer_relocate(struct drm_device *dev,
			     struct eb_objects *eb)
{
	struct drm_i915_gem_object *obj;
	int ret = 0;

	/* This is the fast path and we cannot handle a pagefault whilst
	 * holding the struct mutex lest the user pass in the relocations
	 * contained within a mmaped bo. For in such a case we, the page
	 * fault handler would call i915_gem_fault() and we would try to
	 * acquire the struct mutex again. Obviously this is bad and so
	 * lockdep complains vehemently.
	 */
	pagefault_disable();
	list_for_each_entry(obj, &eb->objects, exec_list) {
		ret = i915_gem_execbuffer_relocate_object(obj, eb);
		if (ret)
			break;
	}
	pagefault_enable();

	return ret;
}